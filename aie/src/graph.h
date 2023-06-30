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

#pragma once
#include <adf.h>
#include "kernel_transform.h"

using namespace adf;

class my_graph: public graph
{

private:
	kernel k_transform;

public:
	input_plio setup_aie;		  // transform parametrs input
	output_plio setup_mutualInfo; // transformed coordinates output
	// output_plio verify;


	my_graph()
	{
		// create kernel
		k_transform = kernel::create(transform);

		// create ports
		setup_aie 	     = input_plio::create("params_in", plio_32_bits, "data/s0.txt");
		setup_mutualInfo = output_plio::create("coords_out", plio_128_bits, "data/result.txt");

		// connect ports and kernels
		connect<stream>(setup_aie.out[0],   k_transform.in[0]);
		connect<stream>(k_transform.out[0], setup_mutualInfo.in[0]);

		// set kernel source and headers
		source(k_transform)  = "src/kernel_transform.cpp";
		headers(k_transform) = {"src/kernel_transform.h","../common/common.h"};

		// set ratio
		runtime<ratio>(k_transform) = 0.9;
	};

};
