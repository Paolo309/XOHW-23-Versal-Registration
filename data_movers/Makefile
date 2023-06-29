#
# Copyright 2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

ECHO=@echo

.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) ""
	$(ECHO) "  make compile"
	$(ECHO) "      Command to generate xo kernel file"
	$(ECHO) ""
	$(ECHO) "  make clean"
	$(ECHO) "      Command to remove all the generated files."

PLATFORM := xilinx_vck5000_gen4x8_xdma_2_202210_1

TARGET := hw

CONFIG = ../default.cfg
include ${CONFIG}

.phony: clean

################## hardware build 

XOCCFLAGS := --platform $(PLATFORM) -t $(TARGET)  -s -g

compile: setup_aie_$(TARGET).xo setup_mutualInfo_$(TARGET).xo 

setup_aie_$(TARGET).xo: ./setup_aie.cpp
	v++ $(XOCCFLAGS) --kernel setup_aie -c -o $@ $<

setup_mutualInfo_$(TARGET).xo: ./setup_mutualInfo.cpp
	v++ $(XOCCFLAGS) --kernel setup_mutualInfo -c -o $@ $<

testbench_setminfo: testbench/testbench_setminfo.cpp
	g++ -std=c++14 -I. -I$(XILINX_HLS)/include -o testbench/$@ $^ -O2 `pkg-config --libs opencv` `pkg-config --cflags opencv`

run_testbench_setminfo:
	@mkdir -p testbench/dataset_output
	@mkdir -p testbench/dataset_output_sw
	cd testbench && ./testbench_setminfo $(TX) $(TY) $(ANG) $(N_COUPLES)

testbench_setupaie: testbench/testbench_setupaie.cpp
	g++ -std=c++14 -I. -I$(XILINX_HLS)/include -o testbench/$@ $^ -O2

run_testbench_setupaie:
	cd testbench && ./testbench_setupaie $(TX) $(TY) $(ANG) $(N_COUPLES)

testbench_all: testbench_setupaie testbench_setminfo
	cd testbench && (./testbench_setupaie $(TX) $(TY) $(ANG) $(N_COUPLES) > ../../aie/data/s0.txt)
	$(warning NOT TESTED: AIE X86 MUST BE COMPILED MANUALLY)
	make -C ../aie aie_simulate_x86
	cp ../aie/x86simulator_output/data/result.txt testbench/coords_in.txt
	@mkdir -p testbench/dataset_output
	@mkdir -p testbench/dataset_output_sw
	cd testbench && ./testbench_setminfo $(TX) $(TY) $(ANG) $(N_COUPLES)

testbench_noaie: testbench_setminfo
	@mkdir -p testbench/dataset_output
	@mkdir -p testbench/dataset_output_sw
	cd testbench && ./testbench_setminfo $(TX) $(TY) $(ANG) $(N_COUPLES)

################## clean up
clean:
	$(RM) -rf *.xo *.xclbin *.xclbin.info *.xclbin.link_summary *.jou *.log *.xo.compile_summary _x .Xil
