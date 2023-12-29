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
int open(const char *file_name);
size_t read(int fd, void *buf, size_t size);
void close(int fd);
void seek(int fd, unsigned position);
unsigned tell(int fd);
bool create (const char *file, unsigned initial_size);
tid_t fork(const char *thread_name, struct intr_frame *f) ;
bool remove(const char *file) ;
int filesize(int fd) ;
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock);

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

  case SYS_OPEN:
  {
    const char *file_name = *((const char **)f->esp + 1);
    f->eax = open(file_name);
    break;
  }

  case SYS_CLOSE:
  {
    int fd = *((int*)f->esp + 1);
    close(fd);
    break;

  }
  case SYS_SEEK:
  {
     int fd = *((int*)f->esp + 1);
     unsigned position = *((unsigned*)f->esp + 2);
     seek(fd, position);
     break;
  }

  case SYS_TELL:
  {
     int fd = *((int*)f->esp + 1);
     f->eax = tell(fd); 
     break;
  }

  case SYS_CREATE:
  {

     const char *file = *(const char **)(f->esp + 4);
     unsigned initial_size = *(unsigned*)(f->esp+8);

      if (file == NULL) {
        exit(-1);
      }

      f->eax = (uint32_t) create(file, initial_size);
      break;

  }
  case SYS_REMOVE:
  {     
       const char *file_name=* ((const char **) f->esp+1);
       f->eax = remove(file_name);
        break;
  }
  case SYS_FILESIZE:
  {
       int fd = *((int*)f->esp + 1);
       f->eax = filesize(fd);
       break;
  }
/*  case SYS_FORK:
  {

      f->R.rax = fork(f->R.rdi, f);
      break;

  }*/
  default:

    break;
  }  
  
}


/*valide the pointer of user mode and kernel can accsess it safely*/
void
valid_user_pointer(const void *ptr){
  
  if(ptr==NULL) exit(-1);
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
  
  if(buf==NULL )
    exit(-1);
  
  for (size_t i = 0; i < size; i++) {
    valid_user_pointer((uint8_t*)buf + i);
  }


  if(fd==STDIN_FILENO){
    uint8_t* buf_byte=(uint8_t*) buf;
    for(size_t i=0; i<size; i++){
      buf_byte[i]=input_getc();

    }
    return size;
  }
  if (fd==STDOUT_FILENO)   return -1;


  struct file *f = process_get_file(fd);
  if (!f) return -1; 
 

    return file_read(f, buf, size);
}

size_t write(int fd, const void *buf, unsigned size){

 valid_user_pointer(buf);
 
 if(fd==STDOUT_FILENO){
    putbuf(buf, size);
    return size;
  }
  
  if (fd == STDIN_FILENO) {
        return -1;
   }

   
   struct file *f = process_get_file(fd);
   if (!f) {
        return -1;
   }
   return file_write(f, buf, size);
}



int
open(const char *file_name)
{
   valid_user_pointer(file_name);

   struct file *file =filesys_open(file_name);

   if(file== NULL) return -1;
   int fd=process_add_file(file);  // add file th fdt
   if (fd==-1){
      file_close(file); return -1;
   }

   return fd;

}


void close(int fd)
{
    struct file *f = process_get_file(fd);
    if (!f)  return;
    file_close(f);
    process_close_file(fd);
}

void seek(int fd, unsigned position)
{
  struct file *f = process_get_file(fd);
  if (!f) return;
  file_seek(f, position);
}


unsigned tell(int fd)
{
  struct file *f = process_get_file(fd);
  if (!f)    return -1; 
  return file_tell(f);
}


bool create(const char *file, unsigned initial_size){

  if (!is_user_vaddr(file)) {
    exit(-1);
  }

  return filesys_create(file, initial_size);
}
bool remove(const char *file) {
   return filesys_remove(file);
}
int filesize(int fd) {
    struct file *f = process_get_file(fd);
    if (!f) {
        return -1; // あるいはエラー処理
    }
    return file_length(f);
}

/*
tid_t fork(const char *thread_name, struct intr_frame *f) {
   return process_fork(thread_name, f);
*/



