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


static state_t *prev;
static FILE *output;
static
void handle_nextstate(state_t *next) {
    /* interpolate to a new an eggciting z coordinate */
    
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
    }
    prev = malloc(sizeof(state_t));
    init_state(prev);
    write_header(output);

    parse(infile, handle_nextstate);

    exit(EXIT_SUCCESS);
}
