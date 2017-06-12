#include <stdio.h>
#include <complex.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define TILE_SIZE 512

static void panic(const char* msg) {
    fprintf(stderr, "panic: %s\n", msg);
    exit(1);
}

static inline uint8_t collatz(double complex z) {
    uint8_t iter = 0;
    while (iter < 16 && cabs(z) < 16) {
        iter++;
        z = (1.0+4.0*z-(1+2.0*z)*ccos(M_PI*z))/4.0;
    }
    return iter*17;
}

int main(int argc, char *argv[]) {
    if (argc != 4){
        panic("bad number of arguments");
    }

    const double complex location = atof(argv[1]) + atof(argv[2])*I;
    const double width = atof(argv[3]);
    const double step_size = width / TILE_SIZE;
    int32_t y;
    for (y=0; y<TILE_SIZE; y++) {
        uint8_t row[TILE_SIZE];
        int32_t x;
        for (x=0; x<TILE_SIZE; x++) {
            double complex c = location + x*step_size + y*step_size*I;
            row[x] = collatz(c);
        }
        (void)fwrite(&row, sizeof(row), 1, stdout);
    }
    return 0;
}