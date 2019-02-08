#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void handler(int sig) {}

int main(int argc, char * argv[]) {
  pid_t childPid;
  struct sigaction sa;

  setbuf(stdout, NULL);

  /* Make stdout unbuffered */

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  childPid = fork();
  if (childPid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  // The following code shows that an orphan process whose group leader exits will not receive SIGHUB.
  //if (childPid > 0) {
  //  printf("Parent exits, then child becomes orphaned.\n");
  //  return 0;
  //}

  if (childPid == 0 && argc > 1) {
    if (setpgid(0, 0) == -1) {
      perror("setpgid");
      exit(EXIT_FAILURE);
    }
  }

  /* Move to new process group */

  // The following code shows that an orphan process whose group leader still lives will receive SIGHUB.
  // Also note, when the group leader's child exits, it will receive a SIGCHLD signal, and it will then prins "%ld: caught SIGHUP\n".
  // After which it continue the for loop to wait for another signal.
  if (childPid == 0) {
    childPid = fork();
    if (childPid > 0) {
      printf("exits\n");
      return 0;
    }
  }

  printf("PID=%ld; PPID=%ld; PGID=%ld; SID=%ld\n",
         (long)getpid(),
         (long)getppid(),
         (long)getpgrp(),
         (long)getsid(0));

  alarm(60);

  /* An unhandled SIGALRM ensures this process will die if nothing else terminates it */ /* Wait for signals */

  for (;;) {
    pause();
    printf("%ld: caught SIGHUP\n", (long)getpid());
  }
}
