#include "priority_queue.h"
#include "../debug.h"

/* Initializes Priority Queue as an empty pq. */
void
pq_init(struct pq *pq)
{
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
  ASSERT(before != NULL && elem != NULL);

  // update id first
  elem->id = before->prev->id + 1; 

  elem->prev = before->prev;
  elem->next = before;
  before->prev->next = elem;
  before->prev = elem;


  // attributes to maintain a max heap
  elem->parent = parent(elem);
  if (elem->parent != NULL)
    {
      if (elem->id == 2 * elem->parent->id)
	elem->parent->left = elem;
      else if (elem->id == 2 * elem->parent->id + 1)
	elem->parent->right = elem;
    }

  elem->left = left(elem);
  if (elem->left != NULL)
    elem->left->parent = elem;

  elem->right = right(elem);
  if (elem->right != NULL)
    elem->right->parent = elem;

  elem->priority = priority;
}


/* Inserts ELEM at the end of LIST, so that it becomes the
   back in LIST. */
void
pq_push_back(struct pq *pq, struct pq_elem *elem, int priority)
{
  pq_insert(pq_tail(pq), elem, priority);
}

/* Removes ELEM from its list and returns the element that
   followed it.  Undefined behavior if ELEM is not in a list.

   It's not safe to treat ELEM as an element in a list after
   removing it.  

   NO NEED to update other ELEMs' id's since pq_remove is
   invoked either when the last ELEM is removed or exchanging two
   ELEMs and neither action will affect others' id's
*/
struct pq_elem *
pq_remove(struct pq_elem *elem)
{
  ASSERT(elem != NULL);
  elem->prev->next = elem->next;
  elem->next->prev = elem->prev;

  if (elem->parent != NULL) 
    {
      if (elem == elem->parent->left)
        elem->parent->left = NULL;
      else
	elem->parent->right = NULL;
    }
 
  if (elem->left != NULL)
    elem->left->parent = NULL;
  if (elem->right != NULL)
    elem->right->parent = NULL;

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


/* Returns the front element in LIST.
   Undefined behavior if LIST is empty. */
struct pq_elem *
pq_front(struct pq *pq)
{
  ASSERT(pq != NULL);
  return pq->head.next;
}

/* Returns the front element in LIST.
   Undefined behavior if LIST is empty. */

struct pq_elem *
pq_back(struct pq *pq)
{
  ASSERT (pq != NULL);
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
  return pq->head.next == &pq->tail;
}

/* calculate one's parent by ID attribute of heap
 * ID(ELEM->PARENT) = ceiling(ID(ELEM) / 2)
 */
struct pq_elem *
parent(struct pq_elem *elem)
{
  ASSERT(elem != NULL);

  struct pq_elem *child = elem;

  int i = elem->id / 2, j;
  for (j = elem->id; j > i; j--)
    {
      if (elem->prev->prev == NULL)
	return NULL;
      elem = elem->prev;
    }
  if (child->id == elem->id * 2)
    elem->left = child;
  else
    elem->right = child;
  return elem;
}

/* calculate one's left child by ID attribute of heap
 * ID(ELEM->left) = 2 * ID(ELEM)
 */
struct pq_elem *
left(struct pq_elem *elem)
{
  ASSERT(elem != NULL);
  int i = elem->id * 2, j;
  for (j = elem->id; j < i; j++)
    {
      if (elem->next->next == NULL)
	return NULL;
      elem = elem->next;
    }
  return elem;
}

/* calculate one's left child by ID attribute of heap
 * ID(ELEM->right) = 2 * ID(ELEM) + 1
 */
struct pq_elem *
right(struct pq_elem *elem)
{
  ASSERT(elem != NULL);
  int i = elem->id * 2 + 1, j;
  for (j = elem->id; j < i; j++)
    {
      if (elem->next->next == NULL)
	return NULL;
      elem = elem->next;
    }
  return elem;
}

/* elect a new root */
void 
max_heapify(struct pq_elem *elem)
{
  ASSERT(elem != NULL);
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
      max_heapify(elem);
    }
}

/* build a max heap on an existing list */
void 
build_max_heap(struct pq *pq)
{
  ASSERT(pq != NULL);
  struct pq_elem *elem;
  for (elem = pq->tail.prev; elem != pq->head.next; elem = elem->prev)
    max_heapify(elem);
}

/* peek root */
struct pq_elem *
heap_maximum(struct pq *pq)
{
  ASSERT(pq != NULL);
  return pq_front(pq);
}

/* pop root */
struct pq_elem *
heap_extract_max(struct pq *pq)
{
  ASSERT(pq != NULL && !pq_empty(pq));

  struct pq_elem *max = pq_front(pq);
  struct pq_elem *lst = pq_back(pq);

  exchange(max, lst);
  pq_remove(max);

  if (max != lst)
    max_heapify(lst);

  return max;
}

/* increase the priority of some ELEM */
void
heap_increase_key(struct pq *pq, struct pq_elem *elem, int key)
{
  ASSERT(elem != NULL);
  elem->priority = key;
  percolate(pq, elem);
}

/* insert a new ELEM into the heap */
void 
max_heap_insert(struct pq *pq, struct pq_elem *elem, int priority)
{
  pq_push_back(pq, elem, priority);
  percolate(pq, elem);
}

/* push ELEM upwards the heap until the max heap property is satisfied */
void 
percolate(struct pq *pq, struct pq_elem *elem)
{
  ASSERT(elem != NULL);

  while (elem != pq_front(pq) && elem->parent->priority < elem->priority)
    {
      exchange(elem, elem->parent);
    }
}

/* exchange two ELEMs in the heap
 * this really need to be done carefully
 */
void exchange(struct pq_elem *fst, struct pq_elem*snd)
{
  ASSERT(fst != NULL && snd != NULL);

  struct pq_elem *fnext = fst->next;
  struct pq_elem *snext = snd->next;

  if (fst == snd)
    {
      return;
    }
  else if (fst->next == snd)
    {
      pq_remove(fst);
      snd->id = snd->prev->id + 1;
      pq_insert(snext, fst, fst->priority);
    }
  else if (snd->next == fst)
    {
      pq_remove(snd);
      fst->id = fst->prev->id + 1;
      pq_insert(fnext, snd, snd->priority);
    }
  else
    {
      pq_remove(fst);
      pq_remove(snd);
      pq_insert(snext, fst, fst->priority);
      pq_insert(fnext, snd, snd->priority);
    }
}
