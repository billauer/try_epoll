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

#define ARRAYSIZE 10

int main(int argc, char **argv) {
  int pollfd;
  int fd;
  struct epoll_event event;
  struct epoll_event event_array[ARRAYSIZE];

  struct {
    uint32_t ecode;
    char *ename;
  } *p, eventcodes[] = {
    { EPOLLIN, "EPOLLIN" },
    { EPOLLPRI, "EPOLLPRI" },
    { EPOLLOUT, "EPOLLOUT" },
    { EPOLLRDNORM, "EPOLLRDNORM" },
    { EPOLLRDBAND, "EPOLLRDBAND" },
    { EPOLLWRNORM, "EPOLLWRNORM" },
    { EPOLLWRBAND, "EPOLLWRBAND" },
    { EPOLLMSG, "EPOLLMSG" },
    { EPOLLERR, "EPOLLERR" },
    { EPOLLHUP, "EPOLLHUP" },
    { EPOLLRDHUP, "EPOLLRDHUP" },

    { EPOLLET, "EPOLLET" }, // Event triggering!
    {},
  };

  const char *fname = "fifo_for_test";

  unlink(fname); // Remove if exists. May fail, don't care.

  if (mkfifo(fname, 0666)) {
    perror("mkfifo");
    return 1;
  }

  pollfd = epoll_create1(0);

  if (pollfd < 0) {
    perror("epoll_create1");
    return 1;
  }

  if ((fd = open(fname, O_RDONLY)) < 0) {
    perror("open FIFO");
    return 1;
  }

  event.data.fd = fd;
  event.events = 0;

  for (p = eventcodes; p->ename; p++)
    event.events |= p->ecode;

  if (epoll_ctl(pollfd, EPOLL_CTL_ADD, fd, &event)) {
    perror("epoll_ctl");
    return 1;
  }

  while (1) {
    int num, i;

    printf("Invoking epoll_wait...\n");
    if ((num = epoll_wait(pollfd, event_array, ARRAYSIZE, -1)) < 0) {
      perror("epoll_wait");
      break;
    }

    printf("epoll_wait returned with %d events.\n", num);

    for (i=0; i<num; i++) {
      printf("Event %d:", i);

      for (p = eventcodes; p->ename; p++)
	if (event_array[i].events & p->ecode)
	  printf(" %s", p->ename);

      printf("\n");
    }
  }

  printf("Quitting main loop!\n");

  if (close(fd)) {
    perror("While closing FIFO");
    return 1;
  }

  if (close(pollfd)) {
    perror("While closing poll fd");
    return 1;
  }

  return 0;
}
