#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "state.h"

State s1, s2;

void usage(char **args) {
    printf("please use : %s inputfilenam outputfilename \n", args[0]);
    printf("\n\nFrostcoder will generate paramerized gcode, where it is easy to edit the frostrate, and do basic tranlation/scaling\n");
}

/* if return < 0 there is an error, else everything is dandy*/
int handle_word(State *state, char *word) {
    if (0 == strlen(word))
        return 0;
    char type = toupper(word[0]); /* UPPERCASE RULESZ */
    char *val = word+1;        /* the rest of the string */
    switch (type) {
    case 'G':
        state->gmode = atof(val);
        break;
    case 'F':
        state->feedrate = atof(val);
        break;
    case 'M':
        state->modal = atoi(val);
        break;
    case 'T':
        state->tool = atoi(val);
        break;

    case 'X':
        state->x = atof(val);
        break;
    case 'Y':
        state->y = atof(val);
        break;
    case 'Z':
        state->z = atof(val);
        break;
    case '\n':
    case ' ':
        /* empty line */
        break;
    default:
        fprintf(stderr, "unknown g-code: %s\n", word);
    }
    return 1;
}

State *handle_line(char *line, State *prev) {
    State *next  = (prev == &s1) ? &s2 : &s1;
    clone_into(next, prev);
    int size = strlen(line);
    assert(size > 0);
    assert(size < LINESIZE);
    strncpy(next->line, line, size);
    char *word;
    char incomment = 0; int comment_lenght = 0;

    word = strtok(line, " ");
    while (word != NULL) {
        /* strip comments */
        char type = word[0];
        if (';' == type || '(' == type) {
            incomment = 1;
        } else if ('\n' == type || ')' == type) {
            incomment = 0;
        }

        if (incomment) {
            comment_lenght += sprintf(next->comment +  comment_lenght, "%s ", word);
        } else {
            handle_word(next, word);
        }
        word = strtok(NULL, " ");
        
    }
    return next;
}

void write_header(FILE *f) {
    fprintf(f, "#1 = 0 (X offset)\n");
    fprintf(f, "#2 = 0 (Y offset)\n");
    fprintf(f, "#3 = 1.0 (XY scale offset)\n");
    fprintf(f, "#4 = 1.0 (Frostrate )\n");
    fprintf(f, "#5 = 1.0 (Feed scale)\n");
}


int main(int argc, char **args) {

    if (argc < 3) {
        usage(args);
        exit(EXIT_FAILURE);
    }
    FILE *input, *output;
    input = fopen(args[1], "r");
    if (input == NULL) {
        perror("unknown open file error");
    }
    output = fopen(args[2], "w");
    if (output == NULL) {
        perror("unknown open file error");
    }
    /* read line by line */
    char buf[512];
    char *line = buf;
    State *prev = &s1;
    State *cur = &s2;
    init_state(prev);
    init_state(cur);
    size_t linesize = 0;
    /* line = (char *)malloc(sizeof(char)*512); */
    int res = 0;
    write_header(output);
    while ( (res = getline(&line, &linesize, input)) > 0 ) {
        prev = cur;
        cur = handle_line(line, prev);
        write_statement(prev, cur, output);
    }
    exit(EXIT_SUCCESS);

}
