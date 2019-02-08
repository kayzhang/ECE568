#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void sigHandler(int sig) {
  printf("Ouch!\n");  // Unsafe (see Section 21.1.2)
}

int main(void) {
  if (signal(SIGINT, sigHandler) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  for (int i = 0;; i++) {
    printf("%d\n", i);
    sleep(3);
  }
}
