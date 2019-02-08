/* Using a pipe to connect ls and wc */
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

  if (pipe(pfd) == -1) {  // Create pipe
    errExit("pipe");
  }

  switch (fork()) {  // Fork child process for ls
    case -1:
      errExit("fork");

    case 0:                       // First child: exec 'ls' to write to pipe
      if (close(pfd[0]) == -1) {  // Read end is unused for ls
        errExit("close");
      }

      /* Duplicate stdout on write end of pipe; close duplicated descriptors */
      // Make sure pfd[1] != STDOUT_FILENO right now
      if (pfd[1] != STDOUT_FILENO) {  // Defensive check
        // if STDOUT_FILENO is open and dup2() failed to close it,
        // the error will be silentltly ignored, so it's better
        // to close STDOUT_FILENO explicitly
        if (close(STDOUT_FILENO) == -1) {
          errExit("close 1");
        }
        // Duplicates pfd[1] and binds to the standard output
        if (dup2(pfd[1], STDOUT_FILENO) == -1) {
          errExit("dup2 1");
        }
        // Close the superfluous descriptors
        if (close(pfd[1]) == -1) {
          errExit("close 2");
        }

        execlp("ls", "ls", (char *)NULL);  // Writes to pipe
        errExit("execlp ls");              // execlp won't return if succeeded

        default:  // Parent falls through to create next child
          break;
      }
  }

  switch (fork()) {  // Fork child process for wc
    case -1:
      errExit("fork");

    case 0:                       // Second child: exec 'wc' to read to pipe
      if (close(pfd[1]) == -1) {  // Write end is unused for wc
        errExit("close");
      }

      /* Duplicate stdin on read end of pipe; close duplicated descriptors */
      // Make sure pfd[0] != STDIN_FILENO right now
      if (pfd[0] != STDIN_FILENO) {  // Defensive check
        // if STDIN_FILENO is open and dup2() failed to close it,
        // the error will be silentltly ignored, so it's better
        // to close STDIN_FILENO explicitly
        if (close(STDIN_FILENO) == -1) {
          errExit("close 3");
        }
        // Duplicates pfd[0] and binds to the standard input
        if (dup2(pfd[0], STDIN_FILENO) == -1) {
          errExit("dup2 3");
        }
        // Close the superfluous descriptors
        if (close(pfd[0]) == -1) {
          errExit("close 4");
        }

        execlp("wc", "wc", "-l", (char *)NULL);  // Reads to pipe
        errExit("execlp wc");                    // execlp won't return if succeeded

        default:  // Parent falls through
          break;
      }

      /* Parent closes unused file descriptors for pipe, and waits for children */
      if (close(pfd[0]) == -1) {
        errExit("close 5");
      }
      if (close(pfd[1]) == -1) {
        errExit("close 6");
      }
      if (wait(NULL) == -1) {
        errExit("wait 1");
      }
      if (wait(NULL) == -1) {
        errExit("wait 2");
      }

      exit(EXIT_SUCCESS);
  }
}
