#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
  int queue=kqueue();
  if(queue == -1) {
    perror("kqueue");
    return 1;
  }

  int fd=open("foo", O_RDONLY);
  if(fd == -1) {
    perror("open");
    return 1;
  }

  struct kevent change;
  EV_SET(&change, fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_ONESHOT,
	 NOTE_DELETE | NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK |
	 NOTE_RENAME | NOTE_REVOKE, 0, 0);

  struct kevent event;

  for(;;) {
    int n=kevent(queue, &change, 1, &event, 1, NULL);
    if(n == -1) {
      perror("kevent");
      return 1;
    }
    else if(n > 0) {
      if(event.fflags & NOTE_DELETE) {
	puts("File was deleted");
	break;
      }
      else if(event.fflags & NOTE_WRITE)
	puts("A write occured");
      else if(event.fflags & NOTE_EXTEND)
	puts("File was extended");
      else if(event.fflags & NOTE_ATTRIB)
	puts("File attribute was changed");
      else if(event.fflags & NOTE_LINK)
	puts("A link to the File was created");
      else if(event.fflags & NOTE_RENAME)
	puts("File was renamed");
      else if(event.fflags & NOTE_REVOKE)
	puts("File access was revoked");
      else
	puts("Unkown");
    }
  }

  close(queue);
  close(fd);
  return 0;
}
