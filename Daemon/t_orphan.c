#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/* This program demonstrates that if a orphan process tries
   to read from or write to the controlling tty, the kernel
   will causes read or write to fail with the error EIO.
   Note, this is true for write when TOSTOP flag is set.
   TOSTOP can be set by run the command: stty tostop. */

int main(void) {
  int pid = 0;
  pid = fork();
  if (pid == 0) {
    printf("I'm a child:\nPlease enter a character:\n");
    int c = fgetc(stdin);
    if (c == EOF) {
      perror("Error type");
    }
    else {
      printf("You enter: %c\n", c);
      return 0;
    }
  }
  else if (pid == -1) {
    printf("fork failed\n");
  }
  else {
    // fork() race condition
    // while (1)
    //   ;
    return 0;
  }
}
