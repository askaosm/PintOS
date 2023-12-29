#include "userprog/syscall.h"
#include <stdio.h>
#include<stdlib.h> //add
#include<string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/init.h" //add
#include "userprog/pagedir.h" //add
#include "userprog/process.h"
#include "devices/input.h"
#include "filesys/file.h"
#include "filesys/filesys.h" //add
static void syscall_handler (struct intr_frame *);
void valid_user_pointer(const void *ptr); //task2
void halt(void);
void exit(int status);
tid_t exec(const char *file_name);
int wait(int pid);
size_t write(int fd, const void *buf, unsigned size);

size_t read(int fd, void *buf, size_t size);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{ 
  valid_user_pointer(f->esp + 4);
  int syscall_number=*(int*)(f->esp);
  switch (syscall_number)
  {
  case SYS_HALT: 
  {  halt(); break;

  }
  
  case SYS_EXIT: 
  { 
    int status=*((int*)f->esp+1);
    exit(status);
    break;
  }
  case SYS_EXEC:
  {
    const char *file_name=*((const char **)f->esp +1);
    f->eax=exec(file_name);//return process id
    break;
  }
  case SYS_WAIT:
  { 
    tid_t child_tid = *((int*)f->esp + 1);
    f->eax = wait(child_tid);

    break;
  }

  case SYS_READ:
  { 
    int fd = *((int*)f->esp+1);
    void *buf=*((void**)f->esp+2);
    unsigned size=*((unsigned*)f->esp+3);
    f->eax=read(fd, buf, size);
    break;
  }

  case SYS_WRITE:
  {
    int fd = *((int*)f->esp+1);
    void *buf=*((void**)f->esp+2);
    size_t size=*((unsigned*)f->esp+3);
    f->eax=write(fd, buf, size);
    break;
  }

  default:

    break;
  }  
  
}


/*valide the pointer of user mode and kernel can accsess it safely*/
void
valid_user_pointer(const void *ptr){
  
  if (!is_user_vaddr(ptr)) {
    exit(-1);
  }
  if(pagedir_get_page(thread_current()->pagedir, ptr)==NULL){
    exit(-1);
  }
}

void 
halt (void) // stop system
{ 
  shutdown_power_off();
}

void
exit(int status){
  struct thread *cur=thread_current(); 
  cur->exit_status=status;
  printf("%s: exit(%d)\n", cur->name , status);
  thread_exit();  //thread stop

}


tid_t
exec(const char *file_name){
  if(file_name==NULL || strlen(file_name)==0){
    //printf("Error: invalid file name\n") ;
    return -1;
  }
  struct file* f = filesys_open(file_name);
  if (f == NULL) {
    //printf("Error: file does not exist\n");
    return -1;
  }
  file_close(f);
  tid_t tid=process_execute(file_name);
  if(tid==TID_ERROR)     return -1;            //process.cfn_copy = palloc_get_page (0); if (fn_copy == NULL) return TID_ERROR;
   
  return tid;  //in user level : tid->tid
}

int
wait(int pid){
  return process_wait(pid);
}

size_t read(int fd, void *buf, size_t size){
  
  if(buf==NULL ){
   // printf("Error: invalid buffer");
    exit(-1);

  }
  if(fd==STDIN_FILENO){
    uint8_t* buf_byte=(uint8_t*) buf;
    for(size_t i=0; i<size; i++){
      buf_byte[i]=input_getc();

    }
    return size;
  }

  else return -1;

}

size_t write(int fd, const void *buf, unsigned size){
//   printf("buf: %p\n",buf);
// //  if (!valid_user_pointer(buf)) {
//   // printf("tesst\n");
//     //exit(-1); 
//   //}
//   if(buf==NULL){
//     printf("Error: invalid buffer\n"); 
//     exit(-1);
//   }
 if(fd==STDOUT_FILENO){
    putbuf(buf, size);
    return size;
  }

   return -1;
}




