#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <tcpd.h>

enum {
  PORT=112233,
  LISTENQ=3,
  EVENTSN=10,
  BUFFERN=1024
};

void dbg(struct kevent *k) {
  printf("ident: %d filter: %s %d flags: %s %s %d udata: %p\n",
	 k->ident,
	 k->filter==EVFILT_READ ? "EVFILT_READ" :
	  k->filter==EVFILT_WRITE ? "EVFILT_WRITE" : "other",
	 k->filter,
	 k->flags & EV_EOF ? "EV_EOF" : "",
	 k->flags & EV_ERROR ? "EV_ERROR" : "",
	 k->flags,
	 k->udata);
}

int main() {
  int queue=kqueue();
  if(queue == -1) {
    perror("kqueue");
    return 1;
  }

  int sock=socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1) {
    perror("socket");
    return 1;
  }

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons((uint16_t)PORT);

  if(bind(sock, (const struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
    perror("bind");
    return 1;
  }

  if(listen(sock, LISTENQ) == -1) {
    perror("listen");
    return 1;
  }

  struct kevent events[EVENTSN];
  EV_SET(events, sock, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0,
	 (void*)0xff);
  if(kevent(queue, events, 1, 0, 0, 0) == -1) {
    perror("kevent - add lsock");
    return 1;
  }

  int n;
  while((n = kevent(queue, 0, 0, events, EVENTSN, 0)) > 0) {
    for(int i=0; i<n; ++i) {
      dbg(&events[i]);
      if(events[i].filter & EVFILT_READ && events[i].udata == (void*)0xff) {
	struct sockaddr_in cliaddr;
	socklen_t clilen=sizeof(cliaddr);
	int newsock=accept(events[i].ident, (struct sockaddr*)&cliaddr,
			   &clilen);
	if(newsock == -1) {
	  if(errno == EINTR)
	    continue;
	  perror("accept");
	  return 1;
	}
	EV_SET(&events[i], newsock, EVFILT_READ,
	       EV_ADD|EV_ENABLE, 0, 0, 0);
	if(kevent(queue, &events[i], 1, 0, 0, 0) == -1) {
	  perror("kevent - add nsock");
	  return 1;
	}
      }
      else if(events[i].filter & EVFILT_READ &&
	      (events[i].flags & EV_EOF ||
	       events[i].flags & EV_ERROR)) {
	events[i].flags = EV_DELETE;
	if(kevent(queue, &events[i], 1, 0, 0, 0) == -1) {
	  perror("kevent - remove");
	  return 1;
	}
	close(events[i].ident);
      }
      else if(events[i].filter & EVFILT_READ) {
	char buffer[BUFFERN];
	ssize_t n=read(events[i].ident, buffer, BUFFERN);
	if(n == -1) {
	  perror("read");
	  return 1;
	}
	if(write(events[i].ident, buffer, n) == -1) {
	  perror("write");
	  return 1;
	}
      }
    }
  }
  if(n == -1) {
    perror("kevent");
    return 1;
  }
  return 0;
}
