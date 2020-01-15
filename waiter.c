#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>

int main(int argc, char **argv) {
  int pollfd;

  unlink("fifo_for_test"); // Remove if exists. May fail, don't care.

  if (mkfifo("fifo_for_test", 0666)) {
    perror("mkfifo");
    return 1;
  }

  pollfd = epoll_create1(0);

  if (pollfd < 0) {
    perror("epoll_create1");
    return 1;
  }

  if (close(pollfd)) {
    perror("While closing poll fd");
    return 1;
  }

  return 0;
}
