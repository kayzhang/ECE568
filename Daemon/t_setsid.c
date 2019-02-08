#define _XOPEN_SOURCE 500
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  // Make sure the process calling setsid() is not
  // process group leader by calling fork()
  int pid = fork();
  if (pid != 0) {  // Exit if parent, or on error
    return 0;
    //exit(EXIT_SUCCESS);
  }

  //for (int i = 0; i < 10000000; i++) {
  //  i = i + 1 - 1;
  //}

  printf("Before setsid:\n");

  printf("PID=%ld, PPID=%ld, PGID=%ld, SID=%ld\n\n",
         (long)getpid(),
         (long)getppid(),
         (long)getpgrp(),
         (long)getsid(0));

  if (setsid() == -1) {
    fprintf(stderr, "Error: setsid\n");
    exit(EXIT_FAILURE);
  }

  printf("After setsid:\n");
  printf("PID=%ld, PPID=%ld, PGID=%ld, SID=%ld\n\n",
         (long)getpid(),
         (long)getppid(),
         (long)getpgrp(),
         (long)getsid(0));

  if (open("/dev/tty", O_RDWR) == -1) {
    fprintf(stderr, "Error: open /dev/tty\n\n");
    // exit(EXIT_FAILURE);
  }

  printf("Although the process has disassociated from the controlling tty,\nit still keeps the "
         "file descriptors inherited from its parent,\nso it can still interact with them (which "
         "is the old terminal).\n\n");

  printf("Another thing to note: since the process now is not job-controlled by the terminal,\nit "
         "can use the terminal(more specificly the file descripters which direct to the terminal) "
         "freely.\nSo, it can read and write to this terminal and race with the terminal's "
         "foreground peocess group.\n\n");

  printf("I'm a daemon:\n Please enter a character:\n");
  int c = fgetc(stdin);
  if (c == EOF) {
    perror("Error type");
  }
  else {
    printf("You enter: %c\n", c);
    return 0;
  }

  exit(EXIT_SUCCESS);
}
