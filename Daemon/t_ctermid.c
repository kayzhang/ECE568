#define _XOPEN_SOURCE 500
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
void checkCTerm(void);

int main(void) {
  printf("Now, the process info is:\n");
  printf("PID=%ld, PGID=%ld, SID=%ld\n", (long)getpid(), (long)getpgrp(), (long)getsid(0));
  printf("This process should have a controlling terminal:\n");
  checkCTerm();
  printf("\n");

  // Get the file descriptor of the controlling terminal
  int fd = open("/dev/tty", O_RDWR);
  if (fd == -1) {
    fprintf(stderr, "Error: open /dev/tty\n");
    exit(EXIT_FAILURE);
  }

  printf(
      "Calling ioctl(fd, TIOCNOTTY) to remove the association with the controlling terminal...\n");
  if (ioctl(fd, TIOCNOTTY) == -1) {
    fprintf(stderr, "Error: ioctl(fd, TIOCNOTTY)\n");
    exit(EXIT_FAILURE);
  }
  printf("Now, the process info is:\n");
  printf("PID=%ld, PGID=%ld, SID=%ld\n", (long)getpid(), (long)getpgrp(), (long)getsid(0));
  printf("We can see that after deassociation with the controlling terminal, the process info does "
         "not change.\nSo I think whether a process has a controlling process has nothing to do "
         "with its id info.");
  checkCTerm();
  printf("We can also see that the ctermid() function will always return /dev/tty no matter the "
         "process has a controlling terminal.\n\n");

  printf("But we can see that now it doesn't have a controlling terminla by open /dev/tty:\n");
  if (open("/dev/tty", O_RDWR) == -1) {
    fprintf(stderr, "Error: open /dev/tty\n");
    exit(EXIT_FAILURE);
  }
}

void checkCTerm(void) {
  char * ttyname = ctermid(NULL);
  if (ttyname != NULL) {
    printf("The return value of ctermid is: %s\n", ttyname);
  }
  else {
    printf("This process does not have a controlling terminal");
  }
}
