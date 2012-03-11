#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "parser.h"
#include "state.h"
#include "eggcoder.h"

static
void usage(char **args) {
    printf("please use : %s inputfilenam outputfilename \n", args[0]);
    printf("\n\nEggcode will generate paramerized gcode to map gcode on to an egg. WILL rewirite Y to A\n");
}

static
void write_header(FILE *f) {
    fprintf(f, "#1 = 0 (X offset)\n");
    fprintf(f, "#2 = 0 (Y offset)\n");
    fprintf(f, "#3 = 1.0 (XY scale offset)\n");
    fprintf(f, "#4 = 1.0 (Frostrate )\n");
    fprintf(f, "#5 = 1.0 (Feed scale)\n");
}
static int points;
double *Xs, *Zs;

static
void read_shape(const char *filename) {
    const size_t max_points = 256;
    Xs = malloc(sizeof(double) * max_points);
    Zs = malloc(sizeof(double) * max_points);
    if (NULL == Xs || NULL == Zs) {
        perror("no mem");
    }
    FILE *f = fopen(filename, "r");
    if (NULL == f) {perror("read shape\n"); abort();}
    unsigned int idx = 0;
    char buf[512];
    double x = 0, z = 0;
    while(NULL != fgets(buf, 512, f) ) {
        if ( 2 != sscanf(buf, "%lf %lf", &x, &z)) {
            fprintf(stderr, "not correct data\n");
        };
        Xs[idx] = x;
        Zs[idx] = z;
        if(idx>1)               
            assert( Xs[idx-1] <= Xs[idx]);
        idx++;
        if (idx == max_points) {
            fprintf(stderr, "AARGH out of range\n");
            abort();
        }
        printf("%f %f from %s\n", x,z, buf); 
    } 
    points = idx;
    if (fclose(f)) {
        perror("read shape\n");
        abort();
    }
}

/* just linear interpolation */
static
double interpolatez(double x) {
    int gt = -1;
    for(int i = 0; i < points; i++) {
        if (x <= Xs[i])  {
            gt = i;
            break;
        }
    }
    assert(gt > 0);
    
    double diff = Xs[gt] - Xs[gt-1];
    double offset = x - Xs[gt-1];
    
    double ratio = offset /diff;
    assert(ratio <= 1 && ratio >= 0);
    
    double res = ratio * Zs[gt-1] +  (1.0-ratio) * Zs[gt];
    return res;
}

static state_t *prev;
static FILE *output;
static
void handle_nextstate(state_t *next) {
    /* interpolate to a new an eggciting z coordinate */
    next->z += interpolatez(next->x);
    /* assume sphere with a center at  */
    write_statement(prev, next, output);
    free(prev);
    prev = next;
}

int main(int argc, char **args) {

    if (argc < 3) {
        usage(args);
        exit(EXIT_FAILURE);
    }
    char *infile = args[1];
    output = fopen(args[2], "w");
    if (output == NULL) {
        perror("unknown open file error");
        abort();
    }
    read_shape("egg.shp");
    prev = malloc(sizeof(state_t));
    init_state(prev);
    write_header(output);

    parse(infile, handle_nextstate);
    fclose(output);
    exit(EXIT_SUCCESS);
}
