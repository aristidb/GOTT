#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <asm/unistd.h>
#include <errno.h>

#include "inotify.h"
#include "inotify-syscalls.h"

#include "event_queue.h"

#define ALL_MASK 0xffffffff
#define EVENTQ_SIZE 128

struct inotify_event *eventq[EVENTQ_SIZE];
int eventq_head = 0;
int eventq_tail = 0;

int open_dev ()
{
	int fd;

	fd = inotify_init ();

	if (fd < 0)
		perror ("inotify_init");

	printf("inotify device fd = %d\n", fd);

	return fd;
}

int close_dev (int fd)
{
	int r;

	if ( (r = close (fd)) < 0) {
		perror ("close (fd) = ");
	}

	return r;
}


void print_mask(int mask)
{
	if (mask & IN_ACCESS)
		printf("ACCESS ");
	if (mask & IN_MODIFY)
		printf("MODIFY ");
	if (mask & IN_ATTRIB)
		printf("ATTRIB ");
	if (mask & IN_CLOSE)
		printf("CLOSE ");
	if (mask & IN_OPEN)
		printf("OPEN ");
	if (mask & IN_MOVED_FROM)
		printf("MOVE_FROM ");
	if (mask & IN_MOVED_TO)
		printf("MOVE_TO ");
	if (mask & IN_DELETE)
		printf("DELETE ");
	if (mask & IN_CREATE)
		printf("CREATE ");
	if (mask & IN_DELETE_SELF)
		printf("DELETE_SELF ");
	if (mask & IN_UNMOUNT)
		printf("UNMOUNT ");
	if (mask & IN_Q_OVERFLOW)
		printf("Q_OVERFLOW ");
	if (mask & IN_IGNORED)
		printf("IGNORED " );

	if (mask & IN_ISDIR)
		printf("(dir) ");
	else
		printf("(file) ");

	printf("0x%08x\n", mask);
}

void print_event (struct inotify_event *event)
{
	printf ("EVENT ON WD=%d\n", event->wd);
	print_mask (event->mask);
	if (event->len)
		printf ("FILENAME=%s\n", event->name);
	printf("\n");
}

void print_events (queue_t q)
{
	struct inotify_event *event;
	while (!queue_empty (q))
	{
		event = queue_front (q);
		queue_dequeue (q);
		print_event (event);
		free (event);
	}
}

int read_events (queue_t q, int fd)
{
	char buffer[16384];
	size_t buffer_i;
	struct inotify_event *pevent, *event;
	ssize_t r;
	size_t event_size;
	int count = 0;

	r = read (fd, buffer, 16384);

	if (r <= 0) {
		perror ("read(fd, buffer, 16384) = ");
		return r;
	}

	buffer_i = 0;
	printf("read = %d\n", r);

	printf("sizeof inotify_event = %d\n", sizeof(struct inotify_event));
	while (buffer_i < r) {
		/* Parse events and queue them ! */
		pevent = (struct inotify_event *)&buffer[buffer_i];
		event_size = sizeof(struct inotify_event) + pevent->len;
		printf("pevent->len = %d\n", pevent->len);
		event = malloc(event_size);
		memmove(event, pevent, event_size);
		queue_enqueue(event, q);
		buffer_i += event_size;
		count++;
	}

	return count;
}

int event_check (int fd)
{
	struct timeval timeout;
	int r;
	fd_set rfds;

	timeout.tv_sec = 4;
	timeout.tv_usec = 0;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	r = select (fd+1, &rfds, NULL, NULL, &timeout);

	return r;
}

int read_and_print_events (queue_t q, int fd)
{
	while (1)
	{
		if (!queue_empty(q))
			print_events (q);

		if (event_check(fd) > 0) {
			int r;

			r = read_events (q, fd);

			if (r < 0)
				break;
		}
	}

	return 0;
}

int watch_dir (int fd, const char *dirname, __u32 mask)
{
	int wd;

	wd = inotify_add_watch (fd, dirname, mask);
	if (wd < 0)
		perror ("inotify_add_watch");

	printf("%s WD=%d\n", dirname, wd);
	return wd;
}

int ignore_wd (int fd, int wd)
{
	int ret;

	ret = inotify_rm_watch (fd, wd);

	if (ret < 0)
		perror("inotify_rm_watch");

	return ret;
}
