/***************************************************************
*
* Configuration header file for Versal 3D Image Registration
*
****************************************************************/
#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef float data_t;
#define DATA_T_BITWIDTH 32

#define DIMENSION 512

#define HIST_PE 16
#define HIST_PE_EXPO 4
#define UNPACK_DATA_BITWIDTH 8
#define INPUT_DATA_BITWIDTH (HIST_PE*UNPACK_DATA_BITWIDTH)
#define NUM_INPUT_DATA (DIMENSION*DIMENSION/(HIST_PE))
#define N_COUPLES_MAX 512
#define J_HISTO_ROWS 256
#define J_HISTO_COLS J_HISTO_ROWS
#define HISTO_ROWS J_HISTO_ROWS
#define INTERVAL_NUMBER 256 // L, amount of levels we want for the binning process, thus at the output

#define SIZE_ROWS 512 // how many rows per aie tile
#define SIZE_COLS 512 // how many columns per aie tile

#define INIT_COLS {-256, -255, -254, -253, -252, -251, -250, -249, -248, -247, -246, -245, -244, -243, -242, -241, -240, -239, -238, -237, -236, -235, -234, -233, -232, -231, -230, -229, -228, -227, -226, -225} // the initial columns for the aie tiles

#define COORD_BITWIDTH 32

#endif
