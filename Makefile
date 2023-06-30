# MIT License

# Copyright (c) 2023 Paolo Salvatore Galfano, Giuseppe Sorrentino

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

ECHO=@echo

.PHONY: help

TARGET := hw
PLATFORM := xilinx_vck5000_gen4x8_xdma_2_202210_1

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make build_hw [TARGET=hw_emu]"
	$(ECHO) ""
	$(ECHO) "  make build_sw"
	$(ECHO) ""
	$(ECHO) "  make clean"
	$(ECHO) ""

CONFIG = default.cfg
include ${CONFIG}

# Build hareware (xclbin) objects
build_hw: compile_krnl_mutual_info compile_data_movers compile_aie hw_link

compile_aie:
	make -C ./aie aie_compile

compile_krnl_histogram:
	make -C ./krnl_histogram compile TARGET=$(TARGET) PLATFORM=$(PLATFORM)

compile_krnl_mutual_info:
	make -C ./mutual_info compile TARGET=$(TARGET) PLATFORM=$(PLATFORM)

compile_data_movers:
	make -C ./data_movers compile TARGET=$(TARGET) PLATFORM=$(PLATFORM)

hw_link:
	make -C ./hw all TARGET=$(TARGET) PLATFORM=$(PLATFORM)

# Build software object
build_sw: 
	make -C ./sw all

config:
	$(info )
	$(info ************ Generating configuration files ************)
	$(info - DIMENSION        $(DIMENSION))
	$(info - N_COUPLES        $(N_COUPLES))
	$(info - N_COUPLES_MAX    $(N_COUPLES_MAX))
	$(info - HIST_PE          $(HIST_PE))
	$(info ********************************************************)
	$(info )
	cd common/generator && python3 generator.py -vts -id $(DIMENSION) -ncm $(N_COUPLES_MAX) -pe $(HIST_PE) -op ../
	mv common/mutual_info.hpp mutual_info/include/hw/mutualInfo
	make -C ./aie generate_input_data
	make -C ./sw switch_dataset

config_and_build:
	echo make config
	echo make build_hw
	echo make build_sw

testbench:
	make -C ./data_movers testbench_all

testbench_c:
	make -C ./aie aie_compile_x86
	make -C ./data_movers testbench_all

testbench_noaie:
	make -C ./data_movers testbench_noaie

NAME := hw_build
pack:
	mkdir -p build/$(NAME)/dataset
	cp -r sw/dataset/** build/$(NAME)/dataset/
	cp sw/host_overlay.exe build/$(NAME)/
	cp hw/overlay_hw.xclbin build/$(NAME)/
	mkdir -p build/$(NAME)/dataset_output
	mkdir -p build/$(NAME)/dataset_sw_output

build_and_pack:
	$(info )
	$(info *********************** Building ***********************)
	$(info - NAME          $(NAME))
	$(info - TARGET        $(TARGET))
	$(info - PLATFORM      $(PLATFORM))
	$(info ********************************************************)
	$(info )
	make config
	make build_hw
	make build_sw
	make pack

# Clean objects
clean: clean_aie clean_mutual_info clean_data_movers clean_hw clean_sw

clean_mutual_info:
	make -C ./mutual_info clean
clean_aie:
	make -C ./aie clean

clean_krnl_histogram:
	make -C ./krnl_histogram clean

clean_data_movers:
	make -C ./data_movers clean

clean_hw:
	make -C ./hw clean

clean_sw: 
	make -C ./sw clean
