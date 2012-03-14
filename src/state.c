#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>


#include "state.h"

state_t *make_state(void) {
    state_t *res = (state_t *) malloc(sizeof(state_t));
    if (NULL == res) { perror("no mem for you");}
    init_state(res);
    return res;
}


void init_state(state_t *s) {
    /* a blank slate */
    s->modal = s->gmode = s->feedrate = s->x = s->y = s->z = s->a = 0;
    s->tool = -1;
    memset( s->line,'\0', LINESIZE);
    memset( s->comment,'\0', LINESIZE);
}

state_t *copy_state(state_t *s) {
    state_t *res = make_state();
    clone_into(res, s);
    return res;
}

void clone_into(state_t *dst, state_t *src){
    if (NULL == src) {
        init_state(dst);
    } else {
        dst->modal = src->modal;
        dst->gmode = src->gmode;
        dst->feedrate = src->feedrate;
        dst->tool = src->tool;
        dst->x = src->x;
        dst->y = src->y;
        dst->z = src->z;
        dst->a = src->a;
        /* clear comments */
        memset( dst->line,'\0', LINESIZE);
        memset( dst->comment,'\0', LINESIZE);
        assert(state_equals(src, dst));
    }

}

/* should not include G0/G1, but aargh for now */
int state_equals(state_t *s1, state_t *s2) {
    return s1->modal == s2->modal &&
        s1->x == s2->x &&
        s1->y == s2->y &&
        s1->z == s2->z &&
        s1->a == s2->a &&
        s1->tool == s2->tool &&
        s1->feedrate == s2->feedrate &&
        s1->gmode == s2->gmode;
}

double dist(state_t *s1, state_t *s2) {
    double dx = s1->x - s2->x;
    double dy = s1->y - s2->y;
    double dz = s1->z - s2->z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

void write_header(FILE *f) {
    fprintf(f, "#1 = 0 (X offset)\n");
    fprintf(f, "#2 = 0 (Y offset)\n");
    fprintf(f, "#3 = 1.0 (XY scale )\n");
    fprintf(f, "#4 = 1.0 (A scale )\n");
    fprintf(f, "#5 = 1.0 (Feed scale)\n");
}

static int parametrize = 0;

/* write the required code to change to the new state */
void write_statement(state_t *cur, state_t *new, FILE *out) {
    char buf[512];
    int idx = 0;
    if (state_equals(cur, new)) {
        /* unless a comment is here */
        if (new->comment[0] != '\0') {
            fprintf(out , "%s\n", new->comment);
        } else {
            fprintf(stderr, "statement with no effect: '%s'\n", new->line);
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
            if (parametrize) 
                idx += sprintf(buf + idx, "X[%f*#3+#1] ", new->x);
            else
                idx += sprintf(buf + idx, "X%f ", new->x);
        }
        if (cur->y != new->y) {
            if (parametrize)
                idx += sprintf(buf + idx, "Y[%f*#3+#2] ", new->y);
            else
                idx += sprintf(buf + idx, "Y%f ", new->y);
        }
        if (cur->z != new->z) {
            /* FIXME parametrize */
            idx += sprintf(buf + idx, "Z%f ", new->z);
        }
        if (cur->a != new->a) {
            idx += sprintf(buf + idx, "A%f ", new->a);
        } 

        if (cur->feedrate != new->feedrate) {
            if (parametrize)
                idx += sprintf(buf + idx, "F[%f*#5] ", new->feedrate);
            else
                idx += sprintf(buf + idx, "F%f ", new->feedrate);
        }

        if (new->comment[0] != '\0') {
            idx += sprintf(buf + idx, "%s ", new->comment);
        }
            
    }
    fprintf(out, "%s\n",buf );
}
