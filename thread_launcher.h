#pragma once

#include <pthread.h>
#include "command_parser.h"
#include "concurrent_list.h"

struct program_state {
  // opened output file
  FILE *outf;
  // the concurrent list/hashtable
  struct list hashtable;
  /* These three are used to count how many
   * threads performing an INSERT command are
   * left, because any DELETE commands must be run
   * only after all INSERT commands complete.
   */
  pthread_mutex_t inserts_mtx;
  pthread_cond_t inserts_cv;
  int inserts_remaining;
};

struct thread_context {
  // individual pthread object
  pthread_t pthread;
  // the command to run for this thread
  struct command cmd;
  // shared state
  struct program_state *state;
};

/**
 * The entrypoint for threads.
 * @param context Pointer to the unique context for this thread.
 *                It must remain valid for the duration of the thread.
 */
void *start_function(void *context);
