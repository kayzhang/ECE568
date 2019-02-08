/* Using a pipe to synchronize multiple processes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]) {
  int pfd[2];  // Process synchronization pipe file descriptors
  int j, dummy;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "Usage: %s sleep-time...\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  setbuf(stdout, NULL);  // Make stdout unbuffered, since we
                         // terminate child with _exit()

  printf("Parent started\n");

  if (pipe(pfd) == -1) {
    errExit("pipe");
  }

  for (j = 1; j < argc; j++) {
    switch (fork()) {
      case -1:
        fprintf(stderr, "fork %d", j);
        exit(EXIT_FAILURE);

      case 0:                       // Child
        if (close(pfd[0]) == -1) {  // Read end is unused for child
          errExit("close");
        }

        /* Child does some work, and lets parent know it's done */
        sleep(atoi(argv[j]));

        printf("Child %d (PID=%ld) closing pipe\n", j, (long)getpid());
        if (close(pfd[1]) == -1) {
          errExit("close");
        }

        /* Child now carries on to do other things... */

        _exit(EXIT_SUCCESS);

      default:  // Parent loops to create next child
        break;
    }
  }

  /* Parent comes here; close write end of pipe so we can see EOF
       after all child processes have closed write end of pipe */

  if (close(pfd[1]) == -1) {  // Write end is unused in parent
    errExit("close");
  }

  /* Parent may do other work, then synchronizes with children */

  if (read(pfd[0], &dummy, 1) != 0) {
    errExit("parent didn't get EOF");
  }
  printf("Parent ready to continue after all children closed write end of pipe\n");
  if (close(pfd[0]) == -1) {  // Write end is unused in parent
    errExit("close");
  }

  /* Parent can now carry on to do other things... */

  exit(EXIT_SUCCESS);
}
