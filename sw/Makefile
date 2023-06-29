# Copyright 2021 Xilinx Inc.
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

ECHO=@echo

.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all"
	$(ECHO) ""
	$(ECHO) "  make clean"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""

CONFIG = ../default.cfg
include ${CONFIG}


.phony: clean


################## software build for XRT Native API code
CXXFLAGS := -std=c++17 -Wno-deprecated-declarations
CXXFLAGS += -I$(XILINX_XRT)/include -I$(XILINX_HLS)/include

LDFLAGS := -L$(XILINX_XRT)/lib
LDFLAGS += $(LDFLAGS) -lxrt_coreutil

LIBS = `pkg-config --libs opencv`
CFLAGS = `pkg-config --cflags opencv`

EXECUTABLE := host_overlay.exe

HOST_SRCS := ./host_code.cpp
HOST_SRCS += ./include/image_utils/image_utils.cpp
HOST_SRCS += ./include/software_mi/software_mi.cpp

DATASET_OUTPUT_DIR := dataset_output
DATASET_SW_OUTPUT_DIR := dataset_sw_output

all: build_sw
build_sw: $(EXECUTABLE)

run_sw:
	mkdir -p $(DATASET_OUTPUT_DIR)
	mkdir -p $(DATASET_SW_OUTPUT_DIR)
	./$(EXECUTABLE)

$(EXECUTABLE): $(HOST_SRCS)
	$(CXX) -o $(EXECUTABLE) $^ $(CXXFLAGS) $(LDFLAGS) $(CFLAGS) $(LIBS)
	@rm -f ./overlay_hw.xclbin
	@rm -f ./overlay_hw_emu.xclbin
	@ln -s ../hw/overlay_hw.xclbin
	@ln -s ../hw/overlay_hw_emu.xclbin

switch_dataset:
	$(info )
	$(info **************** Generating new dataset ****************)
	$(info - DIMENSION        $(DIMENSION))
	$(info - N_COUPLES        $(N_COUPLES))
	$(info ********************************************************)
	$(info )
	rm -f dataset/IM*.png
	cp dataset/size_$(DIMENSION)/IM1.png dataset/
	$(info Duplicating slices $(N_COUPLES) times)
	cd dataset && ./duplicate_slices.sh 2 $(N_COUPLES)

################## clean up
clean:
	$(RM) -r _x .Xil *.ltx *.log *.jou *.info host_overlay.exe *.xo *.xo.* *.str *.xclbin .run
	
