#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

int find_child (int *id, int child_tid, struct thread *cur);
void syscall_init (void);

#endif /* userprog/syscall.h */
