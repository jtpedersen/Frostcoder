#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "state.h"

state_t *make_state(void) {
    state_t *res = (state_t *) malloc(sizeof(state_t));
    if (NULL == res) { perror("no mem for you");}
    return res;
}


void init_state(state_t *s) {
    /* a blank slate */
    s->modal = s->gmode = s->feedrate = s->x = s->y = s->z = s->a = 0;
    s->tool = -1;
    memset( s->line,'\0', LINESIZE);
    memset( s->comment,'\0', LINESIZE);
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
    }

}

/* should not include G0/G1, but aargh for now */
int state_equals(state_t *s1, state_t *s2) {
    return s1->modal == s2->modal &&
        s1->x == s2->x &&
        s1->y == s2->y &&
        s1->z == s2->z &&
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

