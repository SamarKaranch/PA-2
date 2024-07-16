#include "concurrent_list.h"
#include "rwlock.h"
#include "thread_launcher.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void list_wlock(struct list* li);
void list_rlock(struct list* li);
void list_wunlock(struct list* li);
void list_runlock(struct list* li);
uint32_t jenkins(const uint8_t* key, size_t length);

void list_init(struct list *li, struct program_state *state) {
  li->head = NULL;
  li->state = state;
  li->count_acquired = 0;
  li->count_released = 0;
  rwlock_init(&li->rwlock);
}

void list_insert(struct list *li, const char* key, int value) {
  hashRecord *new_node, **p;

  // create and initialize new node
  new_node = malloc(sizeof(hashRecord));
  if(new_node == NULL) {
    return;
  }
  memset(new_node->name, 0, NAME_MAX);
  strncpy(new_node->name, key, NAME_MAX - 1);
  new_node->salary = value;
  new_node->hash = jenkins((const uint8_t *)key, sizeof(key));
  new_node->next = NULL;

  list_wlock(li);

  // advance until we hit a node with a key greater than new_node
  for(p = &li->head; *p != NULL && (*p)->hash <= new_node->hash; p = &(*p)->next) {}
  // insert into the list in ascending hash order
  new_node->next = *p;
  *p = new_node;

  list_wunlock(li);
}

void list_delete(struct list *li, const char* key) {
  uint32_t h;
  hashRecord **p, *temp;

  h = jenkins((const uint8_t *)key, strlen(key));
  list_wlock(li);

  // search for matching key
  for(p = &li->head; *p != NULL; p = &(*p)->next) {
    if((*p)->hash == h) {
      // found it, delete the node and fix node links
      temp = (*p)->next;
      free(*p);
      *p = temp;
      break;
    }
  }

  list_wunlock(li);
}

const hashRecord *list_search(struct list *li, const char *key) {
  uint32_t h;
  hashRecord *node;

  h = jenkins((const uint8_t *)key, strlen(key));
  list_rlock(li);

  // advance until matching key
  for(node = li->head; node != NULL && node->hash != h; node = node->next) {}

  list_runlock(li);
  return node;
}

char *timestamp_string(char *dest) {
  struct timespec ts;

  memset(dest, 0, TIMESTAMP_MAX);
  if(clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
    // error retrieving time
    strcpy(dest, "unknown");
  } else {
    // format timestamp with nanosecond precision
    snprintf(dest, TIMESTAMP_MAX, "%ld.%09ld", (long)ts.tv_sec, (long)ts.tv_nsec);
  }

  return dest;
}

void print_record(FILE *file, const hashRecord *rec) {
  fprintf(file, "%u,%s,%u\n", rec->hash, rec->name, rec->salary);
}

void list_wlock(struct list* li) {
  char tsbuf[TIMESTAMP_MAX];

  rwlock_acquire_writelock(&li->rwlock);
  li->count_acquired += 1;
  timestamp_string(tsbuf);
  fprintf(li->state->outf, "%s,WRITE LOCK ACQUIRED\n", tsbuf);
}

void list_rlock(struct list* li){
  char tsbuf[TIMESTAMP_MAX];

  rwlock_acquire_readlock(&li->rwlock);
  li->count_acquired += 1;
  timestamp_string(tsbuf);
  fprintf(li->state->outf, "%s,READ LOCK ACQUIRED\n", tsbuf);
}

void list_wunlock(struct list* li) {
  char tsbuf[TIMESTAMP_MAX];

  rwlock_release_writelock(&li->rwlock);
  li->count_released += 1;
  timestamp_string(tsbuf);
  fprintf(li->state->outf, "%s,WRITE LOCK RELEASED\n", tsbuf);
}

void list_runlock(struct list* li) {
  char tsbuf[TIMESTAMP_MAX];

  rwlock_release_readlock(&li->rwlock);
  li->count_released += 1;
  timestamp_string(tsbuf);
  fprintf(li->state->outf, "%s,READ LOCK RELEASED\n", tsbuf);
}

// Source: https://en.wikipedia.org/wiki/Jenkins_hash_function
uint32_t jenkins(const uint8_t* key, size_t length) {
  size_t i = 0;
  uint32_t hash = 0;
  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}
