#ifndef PARSER_H_
#define PARSER_H
#include "state.h"

int handle_word(state_t *state, char *word);
state_t *handle_line(char *line, state_t *prev);

int parse(char *filename, void (*callback)(state_t *));

#endif /* !PARSER_H_ */
