#include "thread_launcher.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Format a nanosecond timestamp into dest
static char *timestamp_string(char *dest);

void *start_function(void *_context) {
  struct thread_context *context;
  char tsbuf[20];

  // Cast argument to proper type
  context = (struct thread_context *)_context;

  /* TODO implement each command
   * we can refactor each one into a different
   * function if it makes the code more readable
   */
  switch(context->cmd.type) {
  case CMD_INSERT:
    timestamp_string(tsbuf);
    fprintf(context->state->outf, "%s,INSERT,%s,%d\n", tsbuf, context->cmd.name, context->cmd.value);

    /* rest of the code goes here */

    // decrease the count of remaining INSERT commands
    pthread_mutex_lock(&context->state->inserts_mtx);
    context->state->inserts_remaining -= 1;
    if(context->state->inserts_remaining == 0) {
        // signal DELETE command threads that they may proceed
        timestamp_string(tsbuf);
        fprintf(context->state->outf, "%s,SIGNALING DONE WITH INSERTS\n", tsbuf);
        pthread_cond_broadcast(&context->state->inserts_cv);
    }
    pthread_mutex_unlock(&context->state->inserts_mtx);
    break;

  case CMD_DELETE:
    // wait for all INSERT commands to complete
    pthread_mutex_lock(&context->state->inserts_mtx);
    while(context->state->inserts_remaining != 0) {
        timestamp_string(tsbuf);
        fprintf(context->state->outf, "%s,WAITING ON INSERTS TO COMPLETE\n", tsbuf);
        pthread_cond_wait(&context->state->inserts_cv, &context->state->inserts_mtx);
    }
    pthread_mutex_unlock(&context->state->inserts_mtx);

    /* rest of the code goes here */

    timestamp_string(tsbuf);
    fprintf(context->state->outf, "%s,DELETE,%s\n", tsbuf, context->cmd.name);
    break;

  case CMD_SEARCH:
    timestamp_string(tsbuf);
    fprintf(context->state->outf, "%s,SEARCH,%s\n", tsbuf, context->cmd.name);
    break;

  case CMD_PRINT:
    break;
  }

  // Terminates the thread
  return NULL;
}

char *timestamp_string(char *dest) {
  struct timespec ts;

  if(clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
    // error retrieving time
    strcpy(dest, "unknown");
  } else {
    // format timestamp with nanosecond precision
    sprintf(dest, "%ld.%09ld", (long)ts.tv_sec, (long)ts.tv_nsec);
  }

  return dest;
}
