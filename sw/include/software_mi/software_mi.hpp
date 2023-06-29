#pragma once
#include <chrono>
#include "../image_utils/image_utils.hpp"
#include "../../../mutual_info/include/hw/mutualInfo/entropy.h"
#include "../../../mutual_info/include/hw/mutualInfo/histogram.h"

class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    inline double getElapsedSeconds() {
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        return duration.count();
    }
};

double software_mi(int n_couples, const int TX, const int TY, const float ANG, const std::string &input_path, double* duration_sec);
