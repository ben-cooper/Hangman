#ifndef COMMON_H
#define COMMON_H

void *e_malloc(size_t size);
void *e_calloc(size_t nmemb, size_t size);
void *e_realloc(void *ptr, size_t size);
void e_read(int fd, void *buf, size_t count);
void e_write(int fd, const void *buf, size_t count);

#endif
