/* Read-Write Lock implementation using Semaphores
 * Credit to OSTEP authors.
 * Retrieved from:
 * https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c
 */

#pragma once

#include <semaphore.h>

typedef struct _rwlock_t {
  sem_t writelock;
  sem_t lock;
  int readers;
} rwlock_t;

void rwlock_init(rwlock_t *lock);
void rwlock_acquire_readlock(rwlock_t *lock);
void rwlock_release_readlock(rwlock_t *lock);
void rwlock_acquire_writelock(rwlock_t *lock);
void rwlock_release_writelock(rwlock_t *lock);
