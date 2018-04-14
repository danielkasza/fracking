#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define TILE_SIZE 512

static uint8_t colors_r[256];
static uint8_t colors_g[256];
static uint8_t colors_b[256];

static void panic(const char* msg) {
    fprintf(stderr, "panic: %s\n", msg);
    exit(1);
}

static void set_color(unsigned point, double color[3]) {
    int16_t r = 255*color[0]; r = (r>255) ? 255:r; r = (r<0) ? 0:r;
    int16_t g = 255*color[1]; g = (g>255) ? 255:g; g = (g<0) ? 0:g;
    int16_t b = 255*color[2]; b = (b>255) ? 255:b; b = (b<0) ? 0:b;

    colors_r[point] = r;
    colors_g[point] = g;
    colors_b[point] = b;
}

int main(int argc, char *argv[]) {
    if (argc != 2){
        panic("bad number of arguments");
    }

    int last = -1;
    double last_color_rgb[3];
    char *s = strtok(argv[1], ",");
    do {
        unsigned point;
        uint32_t color;
        if (sscanf(s, "%u+%" SCNx32, &point, &color) != 2) {
            panic("Bad argument.");
        }

        if (point > 255) {
            panic("point must be less than 256");
        }

        if ((last == -1) && (point != 0)) {
            panic("point 0 must be defined");
        }

        if ((last != -1) && (last > point)) {
            panic("must be sorted by point");
        }

        double color_rgb[3] = {
            ((color >> 16)&0xFF)/255.0,
            ((color >>  8)&0xFF)/255.0,
            ((color >>  0)&0xFF)/255.0,
        };

        if (point != 0) {
            unsigned steps = point - last;
            double step_rgb[3] = {
                (color_rgb[0] - last_color_rgb[0]) / steps,
                (color_rgb[1] - last_color_rgb[1]) / steps,
                (color_rgb[2] - last_color_rgb[2]) / steps,
            };


            unsigned i;
            for (i=0; i<steps; i++) {
                double this_rgb[3] = {
                    last_color_rgb[0] + step_rgb[0] * i,
                    last_color_rgb[1] + step_rgb[1] * i,
                    last_color_rgb[2] + step_rgb[2] * i,
                };
                set_color(last+i, this_rgb);
            }
        }

        last = point;
        memcpy(last_color_rgb, color_rgb, sizeof(last_color_rgb));
    } while((s = strtok(NULL, ",")) != NULL);

    if(last != 255) {
        panic("point 255 must be defined");
    }
    set_color(255, last_color_rgb);


    printf("P3\n%u %u\n255\n", TILE_SIZE, TILE_SIZE);

    uint32_t y;
    for (y=0; y<TILE_SIZE; y++) {
        uint8_t row_in[TILE_SIZE];

        if(fread(row_in, sizeof(row_in), 1, stdin) != 1) {
            panic("fread");
        }

        uint32_t x;
        for (x=0; x<TILE_SIZE; x++) {
            printf("%hhu %hhu %hhu  ", colors_r[row_in[x]], colors_g[row_in[x]], colors_b[row_in[x]]);
        }
        printf("\n");

    }

    return 0;
}