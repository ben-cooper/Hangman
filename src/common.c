#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "common.h"

void *e_malloc(size_t size)
{
	void *result = malloc(size);

	if (!result) {
		fprintf(stderr, "malloc: %zu: %s\n", size, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return result;
}

void *e_realloc(void *ptr, size_t size)
{
	void *result = realloc(ptr, size);

	if (!result) {
		fprintf(stderr, "realloc: %zu: %s\n", size, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return result;
}

int e_creat(const char *pathname, mode_t mode)
{
	int fd;

	if ((fd = creat(pathname, mode)) == -1) {
		fprintf(stderr, "creat: %s: %s\n", pathname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return fd;
}

int e_open(const char *pathname, int flags)
{
	int fd;

	if ((fd = open(pathname, flags)) == -1) {
		fprintf(stderr, "open: %s: %s\n", pathname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return fd;
}

void e_close(int fd)
{
	if (close(fd) == -1) {
		fprintf(stderr, "close: %d: %s", fd, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void e_read(int fd, void *buf, size_t count)
{
	if (read(fd, buf, count) == -1) {
		fprintf(stderr, "read: %d: %s", fd, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void e_write(int fd, const void *buf, size_t count)
{
	if (write(fd, buf, count) == -1) {
		fprintf(stderr, "write: %d: %s", fd, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void e_pipe(int pipefd[2])
{
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}
}

off_t end_lseek(int fd)
{
	off_t result;

	if ((result = lseek(fd, 0, SEEK_END)) == -1) {
		fprintf(stderr, "lseek: %d: %s", fd, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return result;
}

void start_lseek(int fd)
{
	if (lseek(fd, 0, SEEK_SET) == -1) {
		fprintf(stderr, "lseek: %d: %s", fd, strerror(errno));
		exit(EXIT_FAILURE);
	}
}
