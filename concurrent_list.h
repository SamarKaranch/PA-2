#pragma once

#include <stdint.h>
#include <stdio.h>
#include "rwlock.h"

// Assignment says name could be up to 50 chars long
#define NAME_MAX 51
#define TIMESTAMP_MAX 20

typedef struct hash_struct
{
  uint32_t hash;
  char name[NAME_MAX];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

struct list {
  // First node in the list
  hashRecord *head;
  // Read-Write lock
  rwlock_t rwlock;
  // Counters for lock acquisitions/releases
  int count_released;
  int count_acquired;
  // Shared state - only needed for writing
  // messages to the output file.
  struct program_state *state;
};

void list_init(struct list *li, struct program_state *state);
void list_insert(struct list *li, const char* key, int value);
void list_delete(struct list *li, const char* key);
const hashRecord *list_search(struct list *li, const char* key);

/**
 * @brief Format a timestamp string
 * @returns @p dest
 */
char *timestamp_string(char *dest);

/**
 * @brief Outputs a record's hash, name, and salary
 */
void print_record(FILE *file, const hashRecord *rec);
