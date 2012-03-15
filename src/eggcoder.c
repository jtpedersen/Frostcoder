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

    vec3 *tmp = make_vec3();
    for(int j = 0; j < data_grid.rows; j++) {
        for(int i = 0; i < data_grid.cols; i++) {
            unsigned int idx = j*data_grid.cols + i;
            assert(idx < data_size);
            /* printf("data for %d(%d,%d)\n", idx, i, j); */
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


/* just linear interpolation */
static
double interpolatez(double x, double y) {
    if (verbose > 2) {
        printf("get val for (%f,%f)\n", x,y);
    }

    vec3 *data = data_grid.data;
    int stride = data_grid.cols;
    int row = -1, col = -1;
    /* find the col */
    for(int i = 0; i < stride; i++) {
        if (x <= data[i].x)  {
            col = i;
            break;
        }
    }
    /* assume 0 outside sample area */
    if (col < 1) return 0;
    /* find the row */
    for(int j = 0; j < data_grid.rows; j++) {
        if (y <= data[(j * stride) + col].y)  {
            row = j;
            break;
        }
    }
    /* assume 0 outside sample area */
    if (row < 1) return 0;

    fprintf(stderr, "got %dx%d\n", col, row);

    /* 4 points to be interpolated
              N_ratio
          p0 -- p1
          |     |  
          p2 -- p3
               S_ratio
       p3 is at col,row
     */
    vec3 p0 = data[(row-1) * stride + col-1];
    vec3 p1 = data[(row-1) * stride + col  ];
    vec3 p2 = data[(row)   * stride + col-1];
    vec3 p3 = data[(row)   * stride + col  ];

    assert(p0.x <= x && p1.x >= x);
    assert(p2.x <= x && p3.x >= x);

    assert(p0.y <= y && p2.y >= y);
    assert(p1.y <= y && p3.y >= y);


    /* ratio between p0/p1 */
    double N_ratio = (x - p0.x) / (p1.x - p0.x);
    assert(N_ratio >= 0 && N_ratio <= 1);
    /* ratio between p2/p3 */
    double S_ratio = (x - p2.x) / (p3.x - p2.x);
    assert(S_ratio >= 0 && S_ratio <= 1);

    /* interpolate linearly on x using N,S ratio to obtain two new points */
    vec3 n_pt = add(scale(p1, N_ratio), scale(p0, 1.0 - N_ratio));
    vec3 s_pt = add(scale(p3, S_ratio), scale(p2, 1.0 - S_ratio));


    /* interpolate on the new points using y  */
    double ratio = (y - n_pt.y ) / (s_pt.y - n_pt.y);
    assert(ratio >= 0 && ratio <= 1);
    double res = n_pt.z * ratio + (1.0 - ratio) * s_pt.z;

    return res;

}

static state_t *prev;
static FILE *output;
static
void handle_nextstate(state_t *next) {
    /* interpolate to a new an eggciting z coordinate */
    next->z = next->z + interpolatez(next->x, next->y);
    /* printf("%f %f %f\n", next->x, next->y, next->z); */

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
        output = fopen(outfile, "w");
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
