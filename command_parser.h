#pragma once

#include <stdio.h>

// Assignment says name could be up to 50 chars long
#define NAME_MAX 51

enum {
  CMD_THREADS,
  CMD_INSERT,
  CMD_DELETE,
  CMD_SEARCH,
  CMD_PRINT
};

struct command {
  int type;
  int value;
  char name[NAME_MAX];
};

/**
 * @brief Parses the next command in the file.
 * @param file Opened file for reading.
 * @param res Result is placed here.
 * @returns 0 On end-of-file
 * @returns 1 On success
 * @returns -1 On failure
 */
int get_next_command(FILE *file, struct command *res);
