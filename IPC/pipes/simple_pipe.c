/* Using a pipe to communicate between a parent and child process */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SIZE 10

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]) {
  int pfd[2];  // Pipe file descriptors
  char buf[BUF_SIZE];
  ssize_t numRead;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "Usage: %s string\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (pipe(pfd) == -1) {  // Create the pipe before fork()
    errExit("pipe");
  }

  switch (fork()) {
    case -1:
      errExit("fork");

    case 0:                       // Child - reads frim pipe
      if (close(pfd[1]) == -1) {  // Close write end which is unused in child
        errExit("close");
      }

      for (;;) {  // Read data from pipe, echo on stdout
        numRead = read(pfd[0], buf, BUF_SIZE);
        if (numRead == -1) {
          errExit("read");
        }
        if (numRead == 0) {
          break;  // End-of-file
        }
        if (write(STDOUT_FILENO, buf, numRead) != numRead) {
          errExit("child - partial/failed write");
        }
      }

      write(STDOUT_FILENO, "\n", 1);
      if (close(pfd[0]) == -1) {
        errExit("close");
      }
      exit(EXIT_SUCCESS);

    default:                      // Parent - writes to pipe
      if (close(pfd[0]) == -1) {  // Close read end which is unused in parent
        errExit("close");
      }

      if (write(pfd[1], argv[1], strlen(argv[1])) != (ssize_t)strlen(argv[1])) {
        errExit("parent - partial/failed write");
      }

      if (close(pfd[1]) == -1) {  // Child will see EOF
        errExit("close");
      }
      wait(NULL);  // Wait the child to finish
      exit(EXIT_SUCCESS);
  }
}
