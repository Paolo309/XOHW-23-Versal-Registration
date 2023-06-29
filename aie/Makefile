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

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
all: aie_compile aie_simulate
all_x86: aie_compile_x86 aie_simulate_x86

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#Clean build products
clean:
	-@rm -rf .Xil .ipcache vivado* *.xpe *.txt *.log
	-@rm -rf Work libadf.a
	-@rm -rf x86simulator_output aiesimulator_output xnwOut .AIE_SIM_CMD_LINE_OPTIONS pl_sample_count* *.html ISS_RPC_SERVER_PORT

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#Compile AIE code
aie_compile: libadf.a

libadf.a: src/*
	@echo "INFO:- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
	@echo "INFO:Running aiecompiler for hw..."
	@rm -rf Work libadf.a
	@mkdir -p Work
	@aiecompiler --target=hw --include="src" --include="../common" --workdir=./Work --heapsize=2048 --stacksize=4096 --xlopt=2 -v src/graph.cpp
	
aie_compile_x86: 
	@echo "INFO:- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
	@echo "INFO:Running aiecompiler for x86sim..."
	@rm -rf Work libadf.a
	@mkdir -p Work
	@aiecompiler --target=x86sim --include="src" --include="../common" --workdir=./Work src/graph.cpp

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#Simulate AIE code
aie_simulate:
	@echo "INFO:- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
	@echo "INFO:Running aiesimulator..."
	@aiesimulator --pkg-dir=./Work --profile

aie_simulate_x86:
	@echo "INFO:- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
	@echo "INFO:Running x86simulator..."
	@x86simulator --pkg-dir=./Work

CONFIG = ../default.cfg
include ${CONFIG}

PI := 3.14159265359
HALF_ROT := 180.0
generate_input_data:
	$(info )
	$(info ********** Generating aie_simulate input data **********)
	$(info - TX, TY, ANG       $(TX), $(TY), $(ANG))
	$(info - N_COUPLES         $(N_COUPLES))
	$(info ********************************************************)
	$(info )
	printf "$(TX)\n$(TY)\n$(shell echo "scale=6; ${ANG}*${PI}/${HALF_ROT}" | bc)\n$(N_COUPLES)\n" > ./data/s0.txt