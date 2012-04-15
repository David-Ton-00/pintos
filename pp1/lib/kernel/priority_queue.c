#include "priority_queue.h"
#include "../debug.h"

/* Initializes Priority Queue as an empty pq. */
void
pq_init(struct pq *pq)
{
  ASSERT (pq != NULL);
  pq->head.prev = NULL;
  pq->head.next = &pq->tail;
  pq->tail.prev = &pq->head;
  pq->tail.next = NULL;

  pq->head.id = 0;
}

/* Inserts ELEM just before BEFORE, which may be either an
   interior element or a tail.  The latter case is equivalent to
   list_push_back(). */
void
pq_insert(struct pq_elem *before, struct pq_elem *elem, int priority)
{
  ASSERT (elem != NULL);

  elem->prev = before->prev;
  elem->next = before;
  before->prev->next = elem;
  before->prev = elem;

  /* attributes to maintain a max heap */
  elem->parent = parent(elem);
  elem->left = left(elem);
  elem->right = right(elem);
  elem->id = before->id + 1; 
  elem->priority = priority;
}

/* Inserts ELEM at the beginning of LIST, so that it becomes the
   front in LIST. */
void
pq_push_front(struct pq *pq, struct pq_elem *elem, int priority)
{
  pq_insert(pq_head(pq), elem, priority);
}

/* Inserts ELEM at the end of LIST, so that it becomes the
   back in LIST. */
void
pq_push_back(struct pq *pq, struct pq_elem *elem, int priority)
{
  pq_insert(pq_back(pq), elem, priority);
}

/* Removes ELEM from its list and returns the element that
   followed it.  Undefined behavior if ELEM is not in a list.

   It's not safe to treat ELEM as an element in a list after
   removing it.  
*/
struct pq_elem *
pq_remove(struct pq_elem *elem)
{
  ASSERT(elem != NULL)
  elem->prev->next = elem->next;
  elem->next->prev = elem->prev;
  return elem->next;
}

/* Removes the front element from LIST and returns it.
   Undefined behavior if LIST is empty before removal. */
struct pq_elem *
pq_pop_front(struct pq *pq)
{
  struct pq_elem *front = pq_front(pq);
  pq_remove(front);
  return front;
}

/* Removes the back element from LIST and returns it.
   Undefined behavior if LIST is empty before removal. */
struct pq_elem *
pq_pop_back(struct pq *pq)
{
  struct pq_elem *back = pq_back(pq);
  pq_remove(back);
  return back;
}

/* Returns the front element in LIST.
   Undefined behavior if LIST is empty. */
struct pq_elem *
pq_front(struct pq *pq)
{
  ASSERT(!pq_empty(pq));
  return pq->head.next;
}

/* Returns the front element in LIST.
   Undefined behavior if LIST is empty. */

struct pq_elem *
pq_back(struct pq *pq)
{
  ASSERT (!pq_empty(pq));
  return pq->tail.prev;
}

struct pq_elem *
pq_head(struct pq *pq)
{
  ASSERT(pq != NULL);
  return &pq->head;
}

struct pq_elem *
pq_tail(struct pq *pq)
{
  ASSERT(pq != NULL);
  return &pq->tail;
}


/* Returns the number of elements in LIST.
   Runs in O(n) in the number of elements. */
size_t
pq_size(struct pq *pq)
{
  struct pq_elem *elem;
  size_t cnt = 0;

  for (elem = pq_front(pq); elem != pq_tail(pq); elem = elem->next)
    cnt++;
  return cnt;
}

/* Returns true if LIST is empty, false otherwise. */
bool
pq_empty(struct pq *pq)
{
  return pq_front(pq) == pq_tail(pq);
}


struct pq_elem *
parent(struct pq_elem *elem)
{
  int i = elem->id / 2, j;
  for (j = elem->id; j > i; j--)
    {
      if (elem->prev->prev == NULL)
	return NULL;
      elem = elem->prev;
    }
  return elem;
}

struct pq_elem *
left(struct pq_elem *elem)
{
  int i = elem->id * 2, j;
  for (j = elem->id; j < i; j++)
    {
      if (elem->next->next == NULL)
	return NULL;
      elem = elem->next;
    }
  return elem;
}

struct pq_elem *
right(struct pq_elem *elem)
{
  int i = elem->id * 2 + 1, j;
  for (j = elem->id; j < i; j++)
    {
      if (elem->next->next == NULL)
	return NULL;
      elem = elem->next;
    }
  return elem;
}

void 
max_heapify(struct pq_elem *elem)
{
  struct pq_elem *left = elem->left;
  struct pq_elem *right = elem->right;
  struct pq_elem *largest = elem;

  if (left != NULL && left->priority > elem->priority)
    largest = left;
  
  if (right != NULL && right->priority > largest->priority)
    largest = right;

  if (largest != elem)
    {
      exchange(elem, largest);
      max_heapify(largest);
    }
}

void 
build_max_heap(struct pq *pq)
{
  struct pq_elem *elem;
  for (elem = pq->tail.prev; elem != pq->head.next; elem = elem->prev)
     max_heapify(elem);
}

struct pq_elem *
heap_maximum(struct pq *pq)
{
  return pq_front(pq);
}

struct pq_elem *
heap_extract_max(struct pq *pq)
{
  return pq_pop_front(pq);
}

void
heap_increase_key(struct pq_elem *elem, int key)
{
  elem->priority = key;
  percolate(elem);
}

void 
max_heap_insert(struct pq *pq, struct pq_elem *elem, int priority)
{
  pq_push_back(pq, elem, priority);
  percolate(elem);
}
void 
percolate(struct pq_elem *elem)
{
  while (elem->prev->prev->prev != NULL && elem->parent->priority < elem->priority)
    {
      exchange(elem, elem->parent);
      elem = elem->parent;
    }

}

void 
exchange(struct pq_elem *fst, struct pq_elem *snd)
{
  int tmp;
  tmp = fst->priority;
  fst->priority = snd->priority;
  snd->priority = tmp;
}




