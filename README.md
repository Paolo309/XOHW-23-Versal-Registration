# XOHW-23-Versal-Registration

## Info & Description
Accelerator for one step of a 3D Image Registration algorithm exploiting the etherogeneity of AMD-Xilinx Versal VCK5000. The acceleration comprises the transformation of the floating volume using AI Engines and the computation of the mutual information accelerated on FPGA.  


|| Information |
|----------------------|-----|
| **Team number**          | xohw23-176 |
| **Project name**         | Heterogeneous Higly Integrated Systems for Image Registration |
| **YouTube's video**      | https://youtu.be/2t6j8ewgJAk |
| **Project's repository** | https://github.com/Paolo309/XOHW-23-Versal-Registration |
| **University name**      | Politecnico di Milano |
| **Participant (email**)  | Paolo Salvatore Galfano (paolosalvatore.galfano@mail.polimi.it) |
| **Participant (email**)  | Giuseppe Sorrentino (giuseppe.sorrentino@mail.polimi.it) |
| **Supervisor name**      | Marco Domenico Santambrogio |
| **Supervisor email**     | marco.santambrogio@polimi.it |
| **Board used**           | AMD-Xilinx Versal VCK5000 |
| **Software version**     | 2022.1 |

## Description of archive
The archive is structured as follows:
```
├─ aie/                # AIE kernel for transfomation
├─ data_movers/        # PL kernels for setting up the AIE and for fetching
├─ mutual_info/        # PL Kernel for computing mutual information
├─ common/             # Common codes and variables
├─ hw/                 # System integration
├─ sw/                 # Host application and dataset
|  └─ dataset/         # Dataset that can be used for testing
└─ Makefile/           # Top-level Makefile to build and run
```
 

## Instructions to build and test project

### Step 1 - Clone the repository
Open a terminal, then clone the repository by running the following command
```shell
git clone https://github.com/Paolo309/XOHW-23-Versal-Registration.git
```
Then, move into the repository with 
```shell
cd XOHW-23-Versal-Registration
```

### Step 2 - Setup the environment
Before building and/or running the framework, run the following script. Notice that this will source Vitis 2022.1, xrt and devtoolset-7
```shell
source ./setup_all.sh
```
Alternatively, if xrt and Vitis are installed in different paths, then run these commands
```shell
source <PATH_TO_XILINX_XRT>/setup.sh
source <PATH_TO_XILINX_VITIS_2022_1>/settings64.sh
scl enable devtoolset-7 bash
```

### Step 3 - Configure
Edit `default.cfg` to configure the accelerator. This step can be skipped, and the accelerator will be configured to support 512x512x512 volumes. Following, there are the configurable parameters:
```
DIMENSION := 512      # IMAGE SIZE
N_COUPLES_MAX := 512  # VOLUME DEPTH 
HIST_PE := 16         # NUMBER OF PROCESSING ELEMENTS (BEST IS 16)
```

### Step 4 - Build
**Optional**: this project already contains a XCLBIN, you can jump the next step.

To build the project, run: 
```shell
make build_and_pack NAME=<build_name> TARTGET=<hw|hw_emu>
```
Note: the default value for `NAME` is "hw_build" and the default for `TARGET` is "hw".


### Step 5 - Run
First, move to the folder containing the buld:
```shell
cd <build_name>
```
Then run the host application my typing
```shell
./host_overlay.exe
```

The volume trasnformed in hardware is stored in the folder `dataset_output` as a sequence of png images, on for each slice. The folder `dataset_sw_output` contains the volume trasformed in software for comparison.
