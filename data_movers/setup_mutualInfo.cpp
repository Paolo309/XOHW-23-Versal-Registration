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

#include <ap_int.h>
#include <hls_stream.h>
#include <hls_math.h>
#include <ap_axi_sdata.h>
#include "../common/common.h"
#include "../mutual_info/include/hw/mutualInfo/mutual_info.hpp"

extern "C" {

constexpr int TRIP_VARIABLE_DIM = DIMENSION;
constexpr int TRIP_VARIABLE_LAYERS_MIN = 32*32;
constexpr int TRIP_VARIABLE_LAYERS_MAX = 512*512*512;
constexpr int TRIP_VARIABLE_LAYERS_AVG = 512*512*256;

constexpr int fifo_in_depth_smi = N_COUPLES_MAX*DIMENSION*DIMENSION;
constexpr int fifo_out_depth_smi = N_COUPLES_MAX*DIMENSION*DIMENSION;

void setup_mutualInfo(
    hls::stream<ap_axis<COORD_BITWIDTH, 0, 0, 0>  >& coords_in, 
    hls::stream<ap_uint<INPUT_DATA_BITWIDTH>>& float_out,
    ap_uint<INPUT_DATA_BITWIDTH>* float_original, 
    ap_uint<INPUT_DATA_BITWIDTH>* float_transformed, 
    int n_couples)
{
#pragma HLS INTERFACE m_axi port=float_original depth=fifo_in_depth_smi offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=float_transformed depth=fifo_out_depth_smi offset=slave bundle=gmem1

#pragma HLS interface axis port=coords_in
#pragma HLS interface axis port=float_out

#pragma HLS INTERFACE s_axilite port=float_original bundle=control
#pragma HLS INTERFACE s_axilite port=float_transformed bundle=control
#pragma HLS INTERFACE s_axilite port=n_couples bundle=control
#pragma HLS interface s_axilite port=return bundle=control

    ap_axis<COORD_BITWIDTH, 0, 0, 0> tmp;
    int32_t i_transformed;

    // they're all set to -1 because they're incremented at the beginning of the loop
    int i = -1;
    int k = -HIST_PE;

    constexpr int DIMENSION_mask = DIMENSION - 1; // used to compute modulo DIMENSION

    const int n_pixels = (DIMENSION * DIMENSION * n_couples) >> HIST_PE_EXPO; // divide by HIST_PE

    loop_fetch_read: for (int q = 0; q < n_pixels; q++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS loop_tripcount min=TRIP_VARIABLE_LAYERS_MIN max=TRIP_VARIABLE_LAYERS_MAX avg=TRIP_VARIABLE_LAYERS_AVG

        // generate the base coordinates
        k += HIST_PE;
        if (k >= n_couples) k = 0;
        if (k == 0)         i = (i + 1) & DIMENSION_mask; // modulo DIMENSION
        
        // at the beginning of a slice, read a new transformed coordinate
        if (k == 0) {
            tmp = coords_in.read();
            i_transformed = tmp.data;
        }

        ap_uint<INPUT_DATA_BITWIDTH> pixel;
        
        // if the transformed coordinate is -1 (out-of-bound), the pixel is set to 0 (black)
        if (i_transformed == -1)
            pixel = 0;
        else
            pixel = float_original[(k + i_transformed) >> HIST_PE_EXPO]; // divide by HIST_PE

        float_out.write(pixel);       // send pixel to mutual information kernel
        float_transformed[q] = pixel; // write transformed volume in memory
    }
}

}
