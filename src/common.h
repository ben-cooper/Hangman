#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>

/**
 * error checking versions of system calls
 */

void *e_malloc(size_t size);
int e_open(const char *pathname, int flags);
void e_close(int fd);
void e_read(int fd, void *buf, size_t count);
void e_write(int fd, const void *buf, size_t count);
void e_pipe(int pipefd[2]);
off_t end_lseek(int fd);
void start_lseek(int fd);

#endif
