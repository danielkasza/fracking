#include <stdio.h>
#include <complex.h>
#include <stdint.h>
#include <stdlib.h>

#define TILE_SIZE 512

static void panic(const char* msg) {
    fprintf(stderr, "panic: %s\n", msg);
    exit(1);
}

static inline uint8_t julia(double complex z, const double complex c) {
    uint8_t retval = 0;
    while (retval < 255 && cabs(z) < 2) {
        retval++;
        z = z*z + c;
    }
    return retval;
}

int main(int argc, char *argv[]) {
    if (argc != 6){
        panic("bad number of arguments");
    }

    const double complex c = atof(argv[1]) + atof(argv[2])*I;
    const double complex location = atof(argv[3]) + atof(argv[4])*I;
    const double width = atof(argv[5]);
    const double step_size = width / TILE_SIZE;
    int32_t y;
    for (y=0; y<TILE_SIZE; y++) {
        uint8_t row[TILE_SIZE];
        int32_t x;
        for (x=0; x<TILE_SIZE; x++) {
            double complex z = location + x*step_size + y*step_size*I;
            row[x] = julia(z, c);
        }
        (void)fwrite(&row, sizeof(row), 1, stdout);
    }
    return 0;
}