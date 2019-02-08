#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fifo_seqnum.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

/* Invoked on exit to delete client FIFO */
static void removeFifo(void) {
  unlink(clientFifo);
}

int main(int argc, char * argv[]) {
  int serverFd, clientFd;
  struct request req;
  struct response resp;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "%s [seq-len...]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Create our FIFO (before sending request, to avoid a race) */

  umask(0);  // So we get the permissions we want
  snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)getpid());
  if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
    fprintf(stderr, "mkfifo %s\n", clientFifo);
    exit(EXIT_FAILURE);
  }

  if (atexit(removeFifo) != 0) {
    errExit("atexit");
  }

  /* Construct request message, open server FIFO, and send request */

  req.pid = getpid();
  req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;

  serverFd = open(SERVER_FIFO, O_WRONLY);
  if (serverFd == -1) {
    fprintf(stderr, "open %s\n", SERVER_FIFO);
    exit(EXIT_FAILURE);
  }

  if (write(serverFd, &req, sizeof(req)) != sizeof(req)) {
    errExit("Can't write to server");
  }

  /* Opem our FIFO, read and display response */

  clientFd = open(clientFifo, O_RDONLY);
  if (clientFd == -1) {
    fprintf(stderr, "open %s\n", clientFifo);
    exit(EXIT_FAILURE);
  }

  if (read(clientFd, &resp, sizeof(resp)) != sizeof(resp)) {
    errExit("Can't read response from server");
  }

  printf("%d\n", resp.seqNum);
  exit(EXIT_SUCCESS);
}
