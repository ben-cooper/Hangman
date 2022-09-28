#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"

void *e_malloc(size_t size)
{
	void *result = malloc(size);

	if (!result) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	return result;
}

int e_open(const char *pathname, int flags)
{
	int fd;

	if ((fd = open(pathname, flags)) == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}

	return fd;
}

void e_close(int fd)
{
	if (close(fd) == -1)
	{
		perror("close");
		exit(EXIT_FAILURE);
	}
}

void e_read(int fd, void *buf, size_t count)
{
	if (read(fd, buf, count) == -1)	{
		perror("read");
		exit(EXIT_FAILURE);
	}
}

void e_write(int fd, const void *buf, size_t count)
{
	if (write(fd, buf, count) == -1) {
		perror("write");
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
		perror("lseek");
		exit(EXIT_FAILURE);
	}

	return result;
}

void start_lseek(int fd)
{
	if (lseek(fd, 0, SEEK_SET) == -1) {
		perror("lseek");
		exit(EXIT_FAILURE);
	}
}
