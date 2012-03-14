#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "vec3.h"
#include "parser.h"
#include "state.h"
#include "eggcoder.h"

static int verbose = 0;

static
void usage(char **args) {
    printf("please use : %s inputfilenam outputfilename \n", args[0]);
    printf("\n\nEggcode will generate paramerized gcode to map gcode on to an egg. WILL rewirite Y to A\n");
}

struct {
    vec3 *data;
    int cols;
    int rows;
} data_grid;


static
void read_file(char *data_file) {
    FILE *f = fopen(data_file, "r");
    if (NULL == f) {
        perror("Could not open data file\n");
        abort();
    }
    
    if (verbose) {
        printf("Will read the file: %s\nassuming a %d by %d grid\n",
               data_file, data_grid.cols, data_grid.rows);
    }
    size_t data_size = data_grid.rows * data_grid.cols;
    data_grid.data = malloc(sizeof(vec3) * data_size);
    if (NULL == data_grid.data) {
        perror("Could not allocate mem for data\n");
        abort();
    }
    /* int c; */
    /* while(EOF != (c = fgetc(f))) { */
    /*     putchar(c); */
    /* } */

    vec3 *tmp = make_vec3();
    for(int j = 0; j < data_grid.rows; j++) {
        for(int i = 0; i < data_grid.cols; i++) {
            unsigned int idx = j*data_grid.cols + i;
            assert(idx < data_size);
            printf("data for %d(%d,%d)\n", idx, i, j);
            char buf[512];
            char *cptr = fgets(buf, 512, f);
            assert(NULL != cptr);
            int read = sscanf(cptr, "%lf %lf %lf", &(tmp->x), &(tmp->y), &(tmp->z));
            if (3 != read) {
                fprintf(stderr, "Arrgh could not read data: %d\n", read);
                abort();
            }

            data_grid.data[idx] = *tmp;
            if (verbose > 2) { print_vec3(*tmp);printf(" @ %d\n", idx); }
        }
    }

    fclose(f);
}

static double *Xs, *Zs;
static int points;

static __attribute__((unused))
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
        /* printf("%f %f from %s\n", x,z, buf);  */
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
    double offset = Xs[gt] - x;
    
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
    next->z = next->z + interpolatez(next->x);
    /* assume sphere with a center at  */
    write_statement(prev, next, output);
    free(prev);
    prev = next;
}

int main(int argc, char **args) {
    /* default grid */
    data_grid.rows = 5;
    data_grid.cols = 5;
    
    char *outfile = NULL;
    char *data_file = NULL;
    
    int opt;
    while ((opt = getopt(argc, args, "d:o:vg:")) != -1) {
        switch (opt) {
        case 'd':
            data_file = optarg;
            break;
        case 'g':
            if (2 != sscanf(optarg, "%dx%d", &data_grid.cols, &data_grid.rows)) {
                printf("wrong formats for geometry\n%s\n", optarg);
            }
            break;
        case 'v':
            verbose++;
            break;
        case 'o':
            outfile = optarg;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
                    args[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        usage(args);
        exit(EXIT_FAILURE);
    }

    char *infile = args[optind];

    if (NULL == outfile)  {
        output = stdout;
    } else {
        output = fopen(args[2], "w");
        if (output == NULL) {
            perror("unknown open file error");
            abort();
        }
    }

    read_file(data_file);


    prev = malloc(sizeof(state_t));
    init_state(prev);
    write_header(output);

    parse(infile, handle_nextstate);
    fclose(output);
    exit(EXIT_SUCCESS);
}
