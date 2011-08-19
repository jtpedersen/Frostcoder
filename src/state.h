#ifndef STATE_H_
#define STATE_H_

#define LINESIZE 512            /* yeah way to make it :S */

/* this is TO simple does not take posistioning, coordinate system or
   the fact that there kan be several active modes at the same og
   different tools */
struct STATE {
    int modal;                  /* M codes */
    int gmode;                  /* G codes */
    int tool;                   /* T ool number */
    float feedrate;               /* F */
    float x,y,z,a;
    char comment[LINESIZE];
    char line[LINESIZE];                 /* for debuggins */
};

typedef struct STATE State;


State *make_state(State *init);

void clone_into(State *dest, State *src);
void init_state(State *s);

void write_statement(State *prev, State *cur, FILE *out);

#endif /* !STATE_H_ */
