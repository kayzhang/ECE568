#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  int filedes[2];

  if (pipe(filedes) == -1) { /* Create the pipe */
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  switch (fork()) { /* Create a child process */
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);

    case 0:                          /* Child */
      if (close(filedes[1]) == -1) { /* Close unused write end */
        perror("close");
        exit(EXIT_FAILURE);
      }

      /* Child now reads from pipe */
      break;

    default:                         /* Parent */
      if (close(filedes[0]) == -1) { /* Close unused read end */
        perror("close");
        exit(EXIT_FAILURE);
      }

      /* Parent now writes to pipe */
      break;
  }
}
