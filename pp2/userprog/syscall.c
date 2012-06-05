#include <user/syscall.h>
#include <stdio.h>
#include <syscall-nr.h>
#include "filesys/filesys.h"
#include "userprog/process.h"
#include "userprog/syscall.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"


static void syscall_handler (struct intr_frame *);
static int get_user(const uint8_t *uaddr);
static bool put_user(uint8_t *udst, uint8_t byte);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  int syscall_number = -1;
  int arg1 = 0, arg2 = 0, arg3 = 0, retval = 0;

  // system call number is in the 32-bit word at the caller's stack pointer 
  if (is_user_vaddr(f->esp))
    {
      syscall_number = get_user(f->esp);
    }
  else
    thread_exit();

  // all syscalls except SYS_HALT have at least one argument
  if (syscall_number != SYS_HALT)
    {
      if (is_user_vaddr(f->esp + 4))
	arg1 = get_user(f->esp + 4);
      else
	thread_exit();
    }

  // the following four syscalls have a second argument
  if (syscall_number == SYS_CREATE ||
      syscall_number == SYS_SEEK ||
      syscall_number == SYS_READ || 
      syscall_number == SYS_WRITE)
    {
      if (is_user_vaddr(f->esp + 8))
	arg2 = get_user(f->esp + 8);
      else
	thread_exit();
    }
  
  // the following two syscalls have a third argument
  if (syscall_number == SYS_WRITE || 
      syscall_number == SYS_READ)
    {
      if (is_user_vaddr(f->esp + 12))
	arg3 = get_user(f->esp + 12);
      else
	thread_exit();
    }

  switch (syscall_number)
    {
    case SYS_CLOSE: close(arg1); break;
    case SYS_CREATE: retval = create((const char *)(arg1), (unsigned)(arg2)); break;
    case SYS_EXEC: retval = exec((const char *)(arg1)); break;
    case SYS_EXIT : exit(arg1); break;
    case SYS_FILESIZE: retval = filesize(arg1); break;
    case SYS_HALT: halt(); break;
    case SYS_OPEN: retval = open((const char *)(arg1)); break;
    case SYS_READ: retval = read(arg1, (void *)(arg2), (unsigned)(arg3)); break;
    case SYS_REMOVE: retval = remove((const char *)(arg1)); break;
    case SYS_SEEK: seek(arg1, (unsigned)(arg2)); break;
    case SYS_TELL: retval = tell(arg1); break;
    case SYS_WAIT: retval = wait((pid_t)(arg1)); break;
    case SYS_WRITE: retval = write(arg1, (const void *)(arg2), (unsigned)(arg3)); break;
    default:
      thread_exit();
    }
  f->eax = retval;
}

static int 
get_user(const uint8_t *uaddr)
{
  int result;
  asm("movl $1f, %0; movzbl %1, %0; 1:"
      : "=&a" (result) : "m" (*uaddr));
  return result;
}

static bool
put_user(uint8_t *udst, uint8_t byte)
{
  int error_code;
  asm("movl $1f, %0; movb %b2, %1; 1:"
      : "=&a" (error_code), "=m" (*udst) : "r" (byte));
  return error_code != -1;
}

void 
halt(void) 
{
  power_off();
}

void 
exit(int status)
{
  struct thread *cur = thread_current();
  struct thread *parent = cur->parent;
  if (parent != NULL)
    {
      struct thread *child = hash_entry(hash_find(&parent->children, &cur->hash_elem), struct thread, hash_elem);
      child->exit_status = status;
    }
  thread_exit();
}

pid_t 
exec(const char *file)
{
  return PID_ERROR;
}
int 
wait (pid_t id)
{
  return process_wait(id);
}
bool 
create(const char *file, unsigned initial_size)
{
  return filesys_create(file, initial_size);
}

bool remove(const char *file)
{
  return filesys_remove(file);
}

int 
open(const char *file)
{
  return -1;
}

int 
filesize(int fd)
{
  return -1;
}

int 
read(int fd, void *buffer, unsigned length)
{
  return -1;
}

int 
write(int fd, const void *buffer, unsigned length)
{
  return -1;
}

void 
seek(int fd, unsigned position)
{
  return -1;
}

unsigned 
tell(int fd)
{
  return 0;
}

void close (int fd)
{
}

