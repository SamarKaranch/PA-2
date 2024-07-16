#include "command_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PARAM_DELIM ","

int get_next_command(FILE *file, struct command *res) {
  char *lineptr, *save, *first, *second, *third;
  size_t linesz;

  // Read a full line from the file
  lineptr = NULL;
  linesz = 0;
  if(getline(&lineptr, &linesz, file) == -1) {
    // Possibly end of file
    return 0;
  }

  // Extract the three words separated by commas
  save = NULL;
  first = strtok_r(lineptr, PARAM_DELIM, &save);
  second = strtok_r(NULL, PARAM_DELIM, &save);
  third = strtok_r(NULL, PARAM_DELIM, &save);

  if(!first || !second || !third) {
    // At least one word is empty - this is an error
    return -1;
  }

  // Parse the type of the command
  if(strcmp(first, "threads") == 0) {
    // This command is an especial case, the integer
    // value is provided in the second word.
    res->type = CMD_THREADS;
    res->value = atoi(second);
    return 1;
  }
  else if(strcmp(first, "insert") == 0) {
    res->type = CMD_INSERT;
  }
  else if(strcmp(first, "delete") == 0) {
    res->type = CMD_DELETE;
  }
  else if(strcmp(first, "search") == 0) {
    res->type = CMD_SEARCH;
  }
  else if(strcmp(first, "print") == 0) {
    res->type = CMD_PRINT;
  } else {
    // Unknown type - error
    return -1;
  }

  // Fill the other two fields of the result
  memset(res->name, 0, NAME_MAX);
  strncpy(res->name, second, NAME_MAX - 1);
  res->value = atoi(third);

  free(lineptr);
  return 1;
}
