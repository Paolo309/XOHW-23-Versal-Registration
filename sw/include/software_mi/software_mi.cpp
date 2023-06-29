#include "software_mi.hpp"

double software_mi(int n_couples, const int TX, const int TY, const float ANG, const std::string &input_path, double* duration_sec) {
    uint8_t* input_ref = new uint8_t[DIMENSION*DIMENSION * n_couples];
    uint8_t* input_flt = new uint8_t[DIMENSION*DIMENSION * n_couples];
    uint8_t* output_flt = new uint8_t[DIMENSION*DIMENSION * n_couples];

    if (read_volume_from_file(input_ref, DIMENSION, n_couples, input_path) == -1) {
        std::cerr << "Could not open file" << std::endl;
        return 1;
    }
    if (read_volume_from_file(input_flt, DIMENSION, n_couples, input_path) == -1) {
        std::cerr << "Could not open file" << std::endl;
        return 1;
    }
    // for(int n = 0; n < DIMENSION*DIMENSION*n_couples; n++)
    // {
    //     input_flt[n] = 100;
    //     input_ref[n] = 100;
    //     output_flt[n] = 255;
    // }

    Timer timer_sw;
    timer_sw.start();
    transform_volume(input_flt, output_flt, TX, TY, ANG, DIMENSION, n_couples);

    // ----------------------------------------------------CALCOLO SOFTWARE DELLA MI-----------------------------------------

    double j_h[J_HISTO_ROWS][J_HISTO_COLS];
    for(int i=0;i<J_HISTO_ROWS;i++){
        for(int j=0;j<J_HISTO_COLS;j++){
            j_h[i][j]=0.0;
        }
    }

    for(int k = 0; k < n_couples; k++) {
        for(int i=0;i<DIMENSION;i++){
            for(int j=0;j<DIMENSION;j++){
                unsigned int a=input_ref[k * DIMENSION * DIMENSION + i * DIMENSION + j];
                unsigned int b=output_flt[k * DIMENSION * DIMENSION + i * DIMENSION + j];
                j_h[a][b]= (j_h[a][b])+1;
            }
        }
    }

    for (int i=0; i<J_HISTO_ROWS; i++) {
        for (int j=0; j<J_HISTO_COLS; j++) {
            j_h[i][j] = j_h[i][j]/(n_couples*DIMENSION*DIMENSION);
        }
    }


    float entropy = 0.0;
    for (int i=0; i<J_HISTO_ROWS; i++) {
        for (int j=0; j<J_HISTO_COLS; j++) {
            float v = j_h[j][i];
            if (v > 0.000000000000001) {
            entropy += v*log2(v);///log(2);
            }
        }
    }
    entropy *= -1;

    double href[ANOTHER_DIMENSION];
    for(int i=0;i<ANOTHER_DIMENSION;i++){
        href[i]=0.0;
    }

    for (int i=0; i<ANOTHER_DIMENSION; i++) {
        for (int j=0; j<ANOTHER_DIMENSION; j++) {
            href[i] += j_h[i][j];
        }
    }

    double hflt[ANOTHER_DIMENSION];
    for(int i=0;i<ANOTHER_DIMENSION;i++){
        hflt[i]=0.0;
    }

    for (int i=0; i<J_HISTO_ROWS; i++) {
        for (int j=0; j<J_HISTO_COLS; j++) {
            hflt[i] += j_h[j][i];
        }
    }


    double eref = 0.0;
    for (int i=0; i<ANOTHER_DIMENSION; i++) {
        if (href[i] > 0.000000000001) {
            eref += href[i] * log2(href[i]);///log(2);
        }
    }
    eref *= -1;


    double eflt = 0.0;
    for (int i=0; i<ANOTHER_DIMENSION; i++) {
        if (hflt[i] > 0.000000000001) {
            eflt += hflt[i] * log2(hflt[i]);///log(2);
        }
    }
    eflt =  eflt * (-1);

    double mutualinfo = eref + eflt - entropy;

    if (duration_sec != NULL)
        *duration_sec = timer_sw.getElapsedSeconds();

    write_volume_to_file(output_flt,DIMENSION,n_couples,"dataset_sw_output/");
    delete[] input_flt;
    delete[] input_ref;
    delete[] output_flt;
    return mutualinfo;
    //printf("Software MI %lf\n", mutualinfo);
}