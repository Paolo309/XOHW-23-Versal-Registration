#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>

// CONFIGURATION: (UN)COMMENT THE DEFINES AS NEEDED

// #define USE_OLD_FORMAT           // use old format for storing input and output volumes
// #define USE_FLOOR                   // truncate transformed coordinates instead of rounding them
// #define USE_FLOAT_INDEX          // compute transformed buffer index without rounding the coordinates first (inaccurate results) 
// #define DEBUG_ACCESSED_INDEXES   // print the index for each read of the input volume
#define TRACK_READS                 // count the ratio of sequential reads over total reads (slower)
#define COMPILE_WITHOUT_DCMTK       // compile without DICOM support (to avoid linking dcmtk libs)

#ifndef COMPILE_WITHOUT_DCMTK
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmimage/diregist.h>
#endif

// POSSIBLE FORMATS
enum ImageFormat { DICOM, PNG };

int read_volume_from_file(uint8_t *volume, const int SIZE, const int N_COUPLES, const std::string &path, const ImageFormat imageFormat = ImageFormat::PNG);
void write_volume_to_file(uint8_t *volume, const int SIZE, const int N_COUPLES, const std::string &path);


uint8_t track_reads(uint8_t *mem, const int index, float *ratio = NULL);

void transform_volume(
    uint8_t *volume_src,
    uint8_t *volume_dest,
    const int TX,
    const int TY,
    const float ANG,
    const int SIZE,
    const int LAYERS
);
