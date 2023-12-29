#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
/*void valid_user_pointer(const void *ptr); //task2
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
int filesize(int fd) ;*/
struct lock filesys_lock;
#endif /* userprog/syscall.h */
