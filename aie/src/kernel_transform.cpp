/*
MIT License

Copyright (c) 2023 Paolo Salvatore Galfano, Giuseppe Sorrentino

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "kernel_transform.h"
#include "common.h"
#include "aie_api/aie.hpp"
#include "aie_api/aie_adf.hpp"
#include "aie_api/utils.hpp"

using namespace std;



alignas(32) const float init_col_const[32] = INIT_COLS;

constexpr float HALF_DIMENSION = DIMENSION / 2.f;

// 
// Generates a stream of transformed coordinates using the parameters passed in params_in.
// params_in must be a stream of 4 floats:
//  - tx: translation along the x axis
//  - ty: translation along the y axis
//  - ang: rotation angle in radians
//  - n_couples: number of slices to be transformed
//
void transform(input_stream<float>* restrict params_in, output_stream<int32>* restrict coords_out) {
    // configuration of the volume transform
    const float tx = readincr(params_in);
    const float ty = readincr(params_in);
    const float ang = readincr(params_in);
    const int n_couples = aie::to_fixed(readincr(params_in), 0);

    // constant values used during transformation phase
    const aie::vector<float, 32> tx_v = aie::broadcast<float, 32>(tx);
    const aie::vector<float, 32> ty_v = aie::broadcast<float, 32>(ty);
    const aie::vector<float, 32> to_center = aie::broadcast<float, 32>(HALF_DIMENSION);
    const aie::vector<float, 32> from_center_tx = aie::sub(to_center, tx_v);
    const aie::vector<float, 32> from_center_ty = aie::sub(to_center, ty_v);
    const float p_cos = cos(ang);
    const float p_sin = sin(ang);
    const float n_sin = -p_sin;

    // constant values used to compute buffer offset from the (c,r) coordinates
    const aie::vector<int32, 32> n_couples_v = aie::broadcast<int32, 32>(n_couples);
    const aie::vector<int32, 32> size_n_couples_v = aie::broadcast<int32, 32>(DIMENSION * n_couples);

    // counters for the columns
    aie::vector<float, 32> init_cols = aie::load_v<32>(init_col_const);
    aie::vector<float, 32> cc_interval = aie::broadcast<float, 32>(32.f);

    int k_index = 0;

    loop_rows: for (int r = -HALF_DIMENSION+k_index*SIZE_ROWS; r < -HALF_DIMENSION+(k_index+1)*SIZE_ROWS; r++) 
    chess_loop_range(32,)
    chess_prepare_for_pipelining
    {
        aie::vector<float, 32> rr = aie::broadcast<float, 32>(r);
        aie::vector<float, 32> cc = init_cols;

        loop_columns: for (int c = 0; c < DIMENSION; c += 32)
        chess_loop_range(32,)
        chess_prepare_for_pipelining
        {
            // 1) translate to (-SIZE/2, -SIZE/2)
            // aie::vector<float, 32> cc_center = aie::sub(cc, to_center);
            // aie::vector<float, 32> rr_center = aie::sub(rr, to_center);

            // 2) rotate of ang radians around the center            
            aie::vector<float, 32> cc_p_cos = aie::mul(cc, p_cos);
            aie::vector<float, 32> cc_p_sin = aie::mul(cc, p_sin);
            aie::vector<float, 32> rr_n_sin = aie::mul(rr, n_sin);
            aie::vector<float, 32> rr_p_cos = aie::mul(rr, p_cos);
            aie::vector<float, 32> cc_center_tra = aie::add(cc_p_cos, rr_n_sin); // c' = c * cos(ang) - r * sin(ang)
            aie::vector<float, 32> rr_center_tra = aie::add(cc_p_sin, rr_p_cos); // r' = c * sin(ang) + r * cos(ang)

            // 3) translate to (-SIZE/2, -SIZE/2) and then to (tx, ty)
            aie::vector<float, 32> cc_tra = aie::add(cc_center_tra, from_center_tx);
            aie::vector<float, 32> rr_tra = aie::add(rr_center_tra, from_center_ty);

            // 4) floor TODO check if another rounding method is needed
            aie::vector<int32, 32> cc_rounded = aie::to_fixed(cc_tra, 0);
            aie::vector<int32, 32> rr_rounded = aie::to_fixed(rr_tra, 0);

            // 5) compute index: I = (r' * SIZE * n_couples) + (c' * n_couples)
            auto cc_rounded_n_couples = aie::mul(cc_rounded, n_couples_v);           // cc_rounded * n_couples
            auto rr_rounded_size_n_couples = aie::mul(rr_rounded, size_n_couples_v); // rr_rounded * SIZE * n_couples

            aie::vector<int32, 32> indexes = aie::add(cc_rounded_n_couples.to_vector<int32>(0), rr_rounded_size_n_couples.to_vector<int32>(0)); // indexes[i] = rr_rounded[i] * SIZE * n_couples + cc_rounded[i] * n_couples
            chess_separator_scheduler();

            // 6) check if the index is out of boundary    
            auto rr_mask_lt = aie::lt(rr_rounded, aie::broadcast<int32, 32>(0));         // rr_mask_lt[i] = rr_rounded[i] < 0
            auto rr_mask_ge = aie::ge(rr_rounded, aie::broadcast<int32, 32>(DIMENSION)); // rr_mask_ge[i] = rr_rounded[i] >= SIZE
            auto cc_mask_lt = aie::lt(cc_rounded, aie::broadcast<int32, 32>(0));         // cc_mask_lt[i] = cc_rounded[i] < 0
            auto cc_mask_ge = aie::ge(cc_rounded, aie::broadcast<int32, 32>(DIMENSION)); // cc_mask_ge[i] = cc_rounded[i] >= SIZE
            auto mask_invalid = rr_mask_ge | cc_mask_lt | rr_mask_lt | cc_mask_ge;

            // 7) set to -1 the indexes that are out of boundary
            aie::vector<int32, 32> validated = aie::select(indexes, aie::broadcast<int32, 32>(-1), mask_invalid); // validated[i] = (mask_invalid[i] == 0 ? indexes[i] : -1;
            
            // // 8) write
            writeincr(coords_out, validated);

            // 9) increase row indexes
            cc = aie::add(cc, cc_interval);
        }
    }
}
