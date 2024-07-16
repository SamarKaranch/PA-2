/* Read-Write Lock implementation using Semaphores
 * Credit to OSTEP authors.
 * Retrieved from:
 * https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c
 */

#include "rwlock.h"

void rwlock_init(rwlock_t *lock) {
  lock->readers = 0;
  sem_init(&lock->lock, 0, 1); 
  sem_init(&lock->writelock, 0, 1); 
}

void rwlock_acquire_readlock(rwlock_t *lock) {
  sem_wait(&lock->lock);
  lock->readers++;
  if (lock->readers == 1) {
    sem_wait(&lock->writelock);
  }
  sem_post(&lock->lock);
}

void rwlock_release_readlock(rwlock_t *lock) {
  sem_wait(&lock->lock);
  lock->readers--;
  if (lock->readers == 0) {
    sem_post(&lock->writelock);
  }
  sem_post(&lock->lock);
}

void rwlock_acquire_writelock(rwlock_t *lock) {
  sem_wait(&lock->writelock);
}

void rwlock_release_writelock(rwlock_t *lock) {
  sem_post(&lock->writelock);
}
