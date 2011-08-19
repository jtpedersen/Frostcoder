#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "state.h"

State *make_state(State *init) {
    State *res = (State *) malloc(sizeof(State));
    if (NULL == res) { perror("no mem for you");}
    clone_into(res, init);
    return res;
}


void init_state(State *s) {
    /* a blank slate */
    s->modal = s->gmode = s->feedrate = s->x = s->y = s->z = s->a = 0;
    s->tool = -1;
    memset( s->line,'\0', LINESIZE);
    memset( s->comment,'\0', LINESIZE);
}

void clone_into(State *dst, State *src){
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
char state_equals(State *s1, State *s2) {
    return s1->modal == s2->modal &&
        s1->x == s2->x &&
        s1->y == s2->y &&
        s1->z == s2->z &&
        s1->tool == s2->tool &&
        s1->feedrate == s2->feedrate &&
        s1->gmode == s2->gmode;
}

float dist(State *s1, State *s2) {
    double dx = s1->x - s2->x;
    double dy = s1->y - s2->y;
    return sqrt(dx*dx+dy*dy);
}


/* write the required code to change to the new state */
void write_statement(State *cur, State *new, FILE *out) {
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
