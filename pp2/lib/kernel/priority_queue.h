#ifndef __LIB_KERNEL_PRIORITY_QUEUE_H
#define __LIB_KERNEL_PRIORITY_QUEUE_H

/* 
 * Priority queue(max heap) is built on doubly linked list.
 * As for imformation of doubly linked list, please turn to list.h
 *
 * max heap property:
 *    a max heap is a complete binary tree
 *    every node has a key which is greater than that of its child if any
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Priority queue(pq) element. */
struct pq_elem 
  {
    struct pq_elem *prev;     /* Previous pq element. */
    struct pq_elem *next;     /* Next pq element. */

    struct pq_elem *parent;   /* Parent elememt */
    struct pq_elem *left;     /* Left child element */
    struct pq_elem *right;    /* Right child element */

     /* add id to find an element's parent and children 
        the head of pq has the default id 0 
        and everyafter inserted element has id one larger than
        that of its previous element
      */
    int id;
    int priority;
  };

/* Priority Queue. */
struct pq 
  {
    struct pq_elem head;      /* Pq head. */
    struct pq_elem tail;      /* Pq tail. */
  };

/* Converts pointer to pq element PQ_ELEM into a pointer to
   the structure that PQ_ELEM is embedded inside.  Supply the
   name of the outer structure STRUCT and the member name MEMBER
   of the pq element.  See the big comment at the top of list.h for an example. */
#define pq_entry(PQ_ELEM, STRUCT, MEMBER)           \
        ((STRUCT *) ((uint8_t *) &(PQ_ELEM)->next     \
                     - offsetof (STRUCT, MEMBER.next)))

/* exact copy of functions of list */

void pq_init(struct pq *pq);


/* List insertion. */
void pq_insert(struct pq_elem *before, struct pq_elem *elem, int priority);
void pq_push_back(struct pq *pq, struct pq_elem *elem, int priority);

/* List removal. */
struct pq_elem *pq_remove(struct pq_elem *elem);
struct pq_elem *pq_pop_front(struct pq *elem);

/* Pq elements. */
struct pq_elem *pq_front(struct pq *pq);
struct pq_elem *pq_back(struct pq *pq);
struct pq_elem *pq_head(struct pq *pq);
struct pq_elem *pq_tail(struct pq *pq);

/* Pq properties. */
size_t pq_size (struct pq *pq);
bool pq_empty (struct pq *pq);

/* implement priority queue(max heap) on the doubly linked list */
struct pq_elem *parent(struct pq_elem *elem);
struct pq_elem *left(struct pq_elem *elem);
struct pq_elem *right(struct pq_elem *elem);
void max_heapify(struct pq_elem *elem);
void build_max_heap(struct pq *pq);
struct pq_elem *heap_maximum(struct pq *pq);
struct pq_elem *heap_extract_max(struct pq *pq);
void heap_increase_key(struct pq *pq, struct pq_elem *elem, int key);
void max_heap_insert(struct pq *pq, struct pq_elem *elem, int priority);
void percolate(struct pq *pq, struct pq_elem *elem);

void exchange(struct pq_elem *fst, struct pq_elem *snd);
#endif /* lib/kernel/priority_queue.h */
