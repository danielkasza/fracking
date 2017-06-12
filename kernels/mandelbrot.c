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

static inline uint8_t mandelbrot(const double complex c) {
    double complex z = 0;
    uint8_t retval = 0;
    while (retval < 255 && cabs(z) < 2) {
        retval++;
        z *= z;
        z += c;
    }
    return retval;
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
            row[x] = mandelbrot(c);
        }
        (void)fwrite(&row, sizeof(row), 1, stdout);
    }
    return 0;
}