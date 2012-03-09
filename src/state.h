#ifndef STATE_H_
#define STATE_H_

#define LINESIZE 512            /* yeah way to make it :S */

/* this is TO simple does not take posistioning, coordinate system or
   the fact that there kan be several active modes at the same og
   different tools */
typedef struct {
    int modal;                  /* M codes */
    int gmode;                  /* G codes */
    int tool;                   /* T ool number */
    float feedrate;               /* F */
    float x,y,z,a;
    char comment[LINESIZE];
    char line[LINESIZE];                 /* for debuggins */
} state_t;



state_t *make_state(void);

void clone_into(state_t *dest, state_t *src);
void init_state(state_t *s);

int state_equals(state_t *s1, state_t *s2);
double dist(state_t *s1, state_t *s2);
void write_statement(state_t *prev, state_t *cur, FILE *out);

#endif /* !STATE_H_ */
