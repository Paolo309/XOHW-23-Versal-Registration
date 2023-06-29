//
// Copyright 2021 Xilinx, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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
