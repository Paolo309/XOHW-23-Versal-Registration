#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <ap_axi_sdata.h>
#include <cmath>
#include "../setup_aie.cpp"
#include <iostream>

void read_from_stream(float *buffer, hls::stream<float> &stream, size_t size) {
    for (unsigned int i = 0; i < size; i++) {
        buffer[i] = stream.read();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "usage: ./testbench_setupaie <tx> <ty> <ang> <n_couples>" << std::endl;
        return -1;
    }

    const float TX = atof(argv[1]);
    const float TY = atof(argv[2]);
    const float ANG_DEG = atof(argv[3]);
    const float ANG = (ANG_DEG * M_PI) / 180.f; ; // radians
    const float n_couples = atof(argv[4]);

    // std::printf("input: tx=%f, ty=%f, ang=%f, n_couples=%f\n", TX, TY, ANG, n_couples);

    hls::stream<float> coords_in;

    setup_aie(TX, TY, ANG, n_couples, coords_in);

    float output[5];
    read_from_stream(output, coords_in, 4); // TODO, sono 5, non 4 (manca la scrittura di kernel_index in setup_aie)

    // std::printf("coords_in: tx=%f, ty=%f, ang=%f, n_couples=%f, k_index=%f\n", output[0], output[1], output[2], output[3], output[4]);
    std::printf("%f\n%f\n%f\n%f\n%f\n", output[0], output[1], output[2], output[3], output[4]);
}
