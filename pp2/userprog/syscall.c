#include <kernel/list.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <user/syscall.h>
#include <syscall-nr.h>
#include "devices/input.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "userprog/syscall.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
static bool invalid_pointer (const void *ptr);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

	 
static void
syscall_handler (struct intr_frame *f) 
{
  	uint32_t syscall_number = -1;
	uint32_t arg1, arg2, arg3, retval;
	uint32_t *addr0, *addr1, *addr2, *addr3;

	if (!invalid_pointer (f->esp))
      {
	addr0 = (uint32_t *) f->esp;
        syscall_number = *addr0;
      }
    else
     	 exit (-1);
     
    if (syscall_number != SYS_HALT)
      {
	if (!invalid_pointer (f->esp + 4))
	       {
		addr1 = (uint32_t *) (f->esp + 4);
                memcpy (&arg1, addr1, sizeof addr1);
	       }
         else
	        exit (-1);
       }

    if (syscall_number == SYS_CREATE || syscall_number == SYS_SEEK || syscall_number == SYS_READ || syscall_number == SYS_WRITE)
       {
	 if (!invalid_pointer (f->esp + 8))
	    {
	        addr2 = (uint32_t *) (f->esp + 8);
	        memcpy (&arg2, addr2, sizeof addr2);
	    }
         else
	        exit (-1);
	}

    if (syscall_number == SYS_WRITE || syscall_number == SYS_READ)
        {
	  if (!invalid_pointer (f->esp + 12))
	    {
	        addr3 = f->esp + 12;
                memcpy (&arg3, addr3, sizeof addr3);
	    }
            else
	        exit (-1);
        }

    switch (syscall_number)
      {
      case SYS_CLOSE: close ((int)arg1); break;
      case SYS_CREATE: retval =  create ((const char*)arg1, (unsigned)arg2); break;
      case SYS_EXEC: retval = exec ((const char *)arg1); break;
      case SYS_EXIT: exit (arg1); break;
      case SYS_FILESIZE: retval = filesize ((int)arg1); break;
      case SYS_HALT: halt (); break;
      case SYS_OPEN: retval = open ((const char *)arg1); break;
      case SYS_READ: retval = read ((int)arg1, (void *)arg2, (unsigned)arg3); break;
      case SYS_REMOVE: retval = remove ((const char *)arg1); break;
      case SYS_SEEK: seek ((int)arg1, (unsigned)arg2); break;
      case SYS_TELL: retval = tell ((int)arg1); break;
      case SYS_WAIT: retval = wait ((pid_t)arg1); break;
      case SYS_WRITE: retval = write ((int)arg1, (const void *)arg2, (unsigned)arg3); break;
      default : 
                exit (-1);
      }
    f->eax = retval;
}


void 
halt (void)
{
    power_off ();
}

void 
exit (int status)
{
    struct thread *cur = thread_current ();
    if (cur->father != NULL)
      {
	struct thread *father = cur->father;
	int i;
        int j = find_child (&i, cur->tid, father);
	father->children_list[j].exit_status = status;
      }
    printf ("%s: exit(%d)\n", cur->file_name, status);
    sema_up (cur->sema_exit);
    thread_exit ();
}

pid_t
exec (const char *file)
{
    if (invalid_pointer (file))
       exit (-1);

    struct thread *cur = thread_current ();
    tid_t tid = process_execute (file);
  
    int i;
    int j = find_child (&i, tid, cur);
   
    sema_down (&cur->children_list[j].sema_load);
  
    if (cur->children_list[j].load_status == false)
             tid = TID_ERROR;
  
    return tid;
}

int 
wait (pid_t pid)
{
    return process_wait (pid);
}

bool
create (const char *file_name, unsigned initial_size)
{
  if (invalid_pointer (file_name))
    exit (-1);
  else
    return filesys_create (file_name, initial_size);
}

bool
remove (const char * file)
{
  if (invalid_pointer (file))
    exit (-1);
  else
    return filesys_remove (file);
}

int 
open (const char *file)
{
   
    if (invalid_pointer (file))
       exit (-1);
    struct file *f;
    struct thread *cur = thread_current ();
    if (cur->fd > OPEN_MAX)
 	exit (-1);	
    else if (!(f = filesys_open (file)))
     	 return -1;
    else
      {
	 int fd = cur->fd;
	 cur->open_file[cur->fd++] = f;
	 if (!strcmp (file, cur->file_name))
	     file_deny_write (f);
	 return fd;
      }
}

int 
filesize (int fd)
{
    if ((fd == STDOUT_FILENO) || (fd == STDIN_FILENO))
	return 0;
    else if ((fd >= 3) && (fd <= OPEN_MAX))
       {	
           struct thread *cur = thread_current ();
           return file_length (cur->open_file[fd]);
       }
    else
	 exit (-1);

}

int
read (int fd, void *buffer, unsigned length)
{
    if (invalid_pointer (buffer))
       exit (-1);  
    struct thread *cur = thread_current ();
    if (fd == STDIN_FILENO)
       {
	 int i = 0;
         uint8_t c;
	 while ((c= input_getc ()) != '\n')
	      {
	    	  memcpy (buffer + i, &c, sizeof c);
		       	 i++;
	      }
	  return i;
       }
    else if ((fd >= 3) && (fd <= OPEN_MAX))
          return file_read (cur->open_file[fd], buffer, length);
    else
	 exit (-1);
}

int 
write (int fd, const void *buffer, unsigned length)
{
    if (invalid_pointer (buffer))
       exit (-1);

    struct thread *cur = thread_current ();

    if (fd == STDOUT_FILENO)
       {
      	   putbuf (buffer, length);
       	   return length;
       }
    else if ((fd >= 3) && (fd <= OPEN_MAX))
        return file_write (cur->open_file[fd], buffer, length);
    else 
         exit (-1);
}

void 
seek (int fd, unsigned position)
{
  struct thread *cur = thread_current ();
  if ((fd >= 3) && (fd <= OPEN_MAX)) 
       file_seek (cur->open_file[fd], position);
  else
        exit (-1);
}

unsigned 
tell (int fd)
{
   struct thread *cur = thread_current ();
   if ((fd >= 3) && (fd <= OPEN_MAX))
         return file_tell (cur->open_file[fd]);
    else
 	 exit (-1);
}

void 
close (int fd)
{ 
    if ((fd >= 3) && (fd <= OPEN_MAX))
      {
           struct thread *cur = thread_current ();
	   struct file *file = cur->open_file[fd];
	   if (file == NULL)
	      exit (-1);
           file_close (file);
	   cur->open_file[fd] = NULL;
      }
}

static bool 
invalid_pointer (const void *ptr)
{
  struct thread *cur = thread_current ();
  return ptr == NULL || !is_user_vaddr (ptr) || pagedir_get_page (cur->pagedir, ptr) == NULL;
}

int
find_child (int *id, int child_tid, struct thread *cur)
{
	int i = 0;
	int j = 0;
	while (i < cur->children_num)
	    {
		if (cur->children_list[j].dead)
		{
		    j++;
		    continue;
		}
	        if (cur->children_list[j].tid == child_tid)
	            break;
		i++;
		j++;
	    }
	*id = i;
	return j;
}
