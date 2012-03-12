#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "parser.h"
#include "state.h"

static
void usage(char **args) {
    printf("please use : %s inputfilenam outputfilename \n", args[0]);
    printf("\n\nFrostcoder will generate paramerized gcode, where it is easy to edit the frostrate, and do basic tranlation/scaling\n");
}

static
void frost_header(FILE *f) {
    fprintf(f, "#1 = 0 (X offset)\n");
    fprintf(f, "#2 = 0 (Y offset)\n");
    fprintf(f, "#3 = 1.0 (XY scale offset)\n");
    fprintf(f, "#4 = 1.0 (Frostrate )\n");
    fprintf(f, "#5 = 1.0 (Feed scale)\n");
}


/* write the required code to change to the new state */
/* FIXME rewirite to not use this redundant code */
static
void write_frostatement(state_t *cur, state_t *new, FILE *out) {
    char buf[512];
    int idx = 0;
    if (state_equals(cur, new)) {
        /* unless a comment is here */
        if (new->comment[0] != '\0') {
            fprintf(out , "%s\n", new->comment);
        } else {
            fprintf(stderr, "statement with no effect: %s", new->line);
        }
        return;
    }
    if (cur->tool != new->tool) {
        sprintf(buf, "T%d", new->tool);
    } else if (cur->modal!=new->modal) {
        sprintf(buf, "M%d", new->modal);
    } else {
        /* it has to be some G thingy */
        idx += sprintf(buf, "G%d ", new->gmode);
        if (cur->x != new->x) {
            idx += sprintf(buf + idx, "X[%f*#3+#1]  ", new->x);
        }
        if (cur->y != new->y) {
            idx += sprintf(buf + idx, "Y[%f*#3+#2] ", new->y);
        }
        if (cur->z != new->z) {
            idx += sprintf(buf + idx, "Z%f ", new->z);
        } else {
            /* its asumed that it either moves up/down as toggling a
             * switch on/off */
            if (cur->z < 0) { /* a simplistic criteria ripe for
                               * configuralibityty */
                float d = dist(cur, new);
                new->a = cur->a + d;
                idx += sprintf(buf + idx, "A[%f*#4*#3] ", new->a);
            }
        }

        if (cur->feedrate != new->feedrate) {
            idx += sprintf(buf + idx, "F[%f*#5] ", new->feedrate);
        }

        if (new->comment[0] != '\0') {
            idx += sprintf(buf + idx, "%s ", new->comment);
        }
            
        /* sprintf(buf + idx, "\n"); */
    }
        
    fprintf(out, "%s\n",buf );

    
}


static state_t *prev;
static FILE *output;
static
void handle_nextstate(state_t *next) {
    write_frostatement(prev, next, output);
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
    frost_header(output);

    parse(infile, handle_nextstate);

    exit(EXIT_SUCCESS);
}
