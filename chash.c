#include "command_parser.h"

#include <stdio.h>

#define INPUT_FILENAME "commands.txt"

int main(void) {
  struct command cmd;
  int err;

  // Open the command for reading
  FILE *file;
  file = fopen(INPUT_FILENAME, "r");
  if(file == NULL) {
    fputs("error opening file\n", stderr);
    return 0;
  }

  while((err = get_next_command(file, &cmd)) == 1) {
    /* Testing */
    printf("got command %d %s %d\n", cmd.type, cmd.name, cmd.value);

    /* TODO implement each function */
    /* Name is in cmd.name */
    /* Integer thread count or salary is in cmd.value */
    switch(cmd.value) {
    case CMD_THREADS:
    case CMD_INSERT:
    case CMD_DELETE:
    case CMD_SEARCH:
    case CMD_PRINT:
      break;
    }
  }
  if(err == -1) {
    fputs("error parsing file\n", stderr);
  }

  return 0;
}
