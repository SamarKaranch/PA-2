#include "command_parser.h"
#include "concurrent_list.h"
#include "thread_launcher.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILENAME "commands.txt"
#define OUTPUT_FILENAME "output.txt"

int main(void) {
  struct command cmd;
  struct program_state state;
  struct thread_context *threads;
  hashRecord *node, *next;
  FILE *file, *outfile;
  int err, i, thread_count;

  // open the file for reading
  file = fopen(INPUT_FILENAME, "r");
  if(file == NULL) {
    fputs("error opening input file\n", stderr);
    return 0;
  }

  // open the output file
  outfile = fopen(OUTPUT_FILENAME, "w");
  if(outfile == NULL) {
    fputs("error opening output file\n", stderr);
    return 0;
  }

  // populate the program state structure
  state.outf = outfile;
  state.inserts_remaining = 0;
  list_init(&state.hashtable, &state);
  pthread_mutex_init(&state.inserts_mtx, NULL);
  pthread_cond_init(&state.inserts_cv, NULL);

  // first command must be the "threads" command
  // providing the number of remaining commands
  err = get_next_command(file, &cmd);
  if(err != 1 || cmd.type != CMD_THREADS) {
    // could not parse the first command,
    // or is not the "threads" command
    fputs("invalid input file\n", stderr);
    return 0;
  }

  // allocate enough threads
  thread_count = cmd.value;
  threads = calloc(thread_count, sizeof(struct thread_context));
  if(threads == NULL) {
    fputs("bad malloc\n", stderr);
    return 0;
  }

  // read remaining commands
  for(i = 0; i < thread_count && get_next_command(file, &cmd) == 1; i++) {
    // initialize thread context
    threads[i].cmd = cmd;
    threads[i].state = &state;
    // we must keep track of the total remaining
    // INSERT commands because DELETE commands
    // will wait for all of them to complete
    if(cmd.type == CMD_INSERT) {
      state.inserts_remaining += 1;
    }
  }
  if(i != thread_count) {
    // we couldn't read all the commands from the file
    fputs("error parsing file\n", stderr);
    return 0;
  }

  // launch all the threads
  for(i = 0; i < thread_count; i++) {
    // threads will run the start_function from thread_launcher.c
    // and will pass their context as the argument
    pthread_create(&threads[i].pthread, NULL, start_function, (void *)&threads[i]);
  }

  // join and wait for all threads to complete
  for(i = 0; i < thread_count; i++) {
    pthread_join(threads[i].pthread, NULL);
  }

  // Final print and destroy list nodes
  fprintf(outfile, "\nNumber of lock acquisitions: %d\n", state.hashtable.count_acquired);
  fprintf(outfile, "Number of lock releases: %d\n", state.hashtable.count_released);
  for(node = state.hashtable.head; node != NULL; node = next) {
    next = node->next;
    print_record(outfile, node);
    free(node);
    node = next;
  }

  free(threads);
  fclose(outfile);
  fclose(file);
  return 0;
}
