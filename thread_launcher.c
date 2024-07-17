#include "thread_launcher.h"
#include "concurrent_list.h"

#include <pthread.h>
#include <stdio.h>

void *start_function(void *_context) {
  struct thread_context *context;
  hashRecord *found, record_copy;
  char tsbuf[TIMESTAMP_MAX];

  // Cast argument to proper type
  context = (struct thread_context *)_context;

  switch(context->cmd.type) {
  case CMD_INSERT:
    timestamp_string(tsbuf);
    fprintf(context->state->outf, "%s,INSERT,%s,%d\n", tsbuf, context->cmd.name, context->cmd.value);
    list_insert(&context->state->hashtable, context->cmd.name, context->cmd.value);

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

    timestamp_string(tsbuf);
    fprintf(context->state->outf, "%s,DELETE,%s\n", tsbuf, context->cmd.name);
    list_delete(&context->state->hashtable, context->cmd.name);
    break;

  case CMD_SEARCH:
    timestamp_string(tsbuf);
    fprintf(context->state->outf, "%s,SEARCH,%s\n", tsbuf, context->cmd.name);

    found = list_search(&context->state->hashtable, context->cmd.name, &record_copy);
    if(found != NULL) {
      print_record(context->state->outf, found);
    } else {
      fputs("No Record Found\n", context->state->outf);
    }
    break;

  case CMD_PRINT:
    // Manually lock the list before iterating through the nodes
    list_rlock(&context->state->hashtable);
    for(found = context->state->hashtable.head; found != NULL; found = found->next) {
        print_record(context->state->outf, found);
    }
    list_runlock(&context->state->hashtable);
    break;
  }

  // Terminates the thread
  return NULL;
}
