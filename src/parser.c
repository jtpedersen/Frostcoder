#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "state.h"

#include "parser.h"


/* if return < 0 there is an error, else everything is dandy*/
int handle_word(state_t *state, char *word) {
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

state_t *handle_line(char *line, state_t *prev) {
    /* printf("look at: %s \n", line); */
    state_t *next  = malloc(sizeof(state_t));
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

int parse(char *filename, void (*callback)(state_t *)) {
    FILE *input;
    input = fopen(filename, "r");
    if (input == NULL) {
        perror("unknown open file error");
    }

    state_t *prev = malloc(sizeof(state_t));
    init_state(prev);

    int state_changes = 0;
    /* read line by line */
    char buf[512];
    char *line = buf;
    size_t linesize = 0;
    int res = 0;
    while ( (res = getline(&line, &linesize, input)) > 0 ) {
        state_t *cur  = handle_line(line, prev);
        state_t *next = make_state();
        clone_into(next, cur);
        callback(next);
        free(prev);
        prev = cur;
        state_changes++;
    }
    return state_changes;
}
