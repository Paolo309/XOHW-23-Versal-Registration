/**********
© Copyright 2020 Xilinx, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**********/


#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>


extern "C" {

void setup_aie(float tx, float ty, float ang,float n_couples, hls::stream<float>& s) {
	#pragma HLS interface axis port=s
	
	#pragma HLS interface s_axilite port=tx bundle=control
	#pragma HLS interface s_axilite port=ty bundle=control
	#pragma HLS interface s_axilite port=ang bundle=control
	#pragma HLS interface s_axilite port=n_couples bundle=control
	#pragma HLS interface s_axilite port=return bundle=control

	s.write(tx);
	s.write(ty);
	s.write(ang);
	s.write(n_couples);
}
}
