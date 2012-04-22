#ifndef THREADS_SYNCH_H
#define THREADS_SYNCH_H

#include <list.h>
#include <priority_queue.h>
#include <stdbool.h>

/* A counting semaphore. */
struct semaphore 
  {
    unsigned value;             /* Current value. */
    // struct pq waiters;
    struct list waiters;
  };

void sema_init (struct semaphore *, unsigned value);
void sema_down (struct semaphore *);
bool sema_try_down (struct semaphore *);
void sema_up (struct semaphore *);
void sema_self_test (void);

/* Lock. */
struct lock 
  {
    struct thread *holder;      /* Thread holding lock (for debugging). */
    struct semaphore semaphore; /* Binary semaphore controlling access. */
    bool donated;
    struct thread *doner;
  };

void lock_init (struct lock *);
void lock_acquire (struct lock *);
bool lock_try_acquire (struct lock *);
void lock_release (struct lock *);
bool lock_held_by_current_thread (const struct lock *);

/* Condition variable. */
struct condition 
  {
    //  struct pq waiters;
    struct list waiters;
  };

void cond_init (struct condition *);
void cond_wait (struct condition *, struct lock *);
void cond_signal (struct condition *, struct lock *);
void cond_broadcast (struct condition *, struct lock *);
bool cond_greater_priority(const struct list_elem *a, const struct list_elem *b, void *aux);

/* One semaphore in a list. */
struct semaphore_elem 
{
  // struct pq_elem elem;
  struct list_elem elem;
  struct semaphore semaphore;         /* This semaphore. */
  struct thread *t;                         
};

/* Optimization barrier.

   The compiler will not reorder operations across an
   optimization barrier.  See "Optimization Barriers" in the
   reference guide for more information.*/
#define barrier() asm volatile ("" : : : "memory")

#endif /* threads/synch.h */
