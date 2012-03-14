/* A short to split moves, so that each segmente has a maximum
 * length */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "vec3.h"
#include "parser.h"
#include "state.h"

static
void usage(char **args) {
    printf("please use : %s inputfilenam \n", args[0]);
}

static inline
vec3 state_to_vec(state_t *s) {
    vec3 res = {s->x, s->y, s->z};
    return res;
}

static double max_dist = .4;

static state_t *prev;
static FILE *output;
static
void handle_nextstate(state_t *next) {
    /* interpolate to a new an eggciting z coordinate */
    state_t *cur = copy_state(next);
    vec3 origo = state_to_vec(prev);
    vec3 pt = state_to_vec(cur);
    vec3 dir = normalize(sub(pt, origo));
    vec3 offset = scale(dir, max_dist);

    while (dist(prev, cur) >  max_dist) {
        /* printf("split because %f\n", dist(prev, cur)); */
        /* print_vec3(offset); */
        /* printf("\n"); */
        cur->x = prev->x + offset.x;
        cur->y = prev->y + offset.y;
        cur->z = prev->z + offset.z;
        write_statement(prev, cur, output);
        free(prev);
        prev = cur;
        cur = copy_state(next);
    }
    write_statement(prev, next, output);
    free(prev);
    free(cur);
    prev = next;
}

int main(int argc, char **args) {

    if (argc < 2) {
        usage(args);
        exit(EXIT_FAILURE);
    }
    char *infile = args[1];
    output = stdout; /* fopen(args[2], "w");  */
    if (output == NULL) {
        perror("unknown open file error");
        abort();
    }
    prev = malloc(sizeof(state_t));
    init_state(prev);
    write_header(output);

    parse(infile, handle_nextstate);
    fclose(output);
    exit(EXIT_SUCCESS);
}
