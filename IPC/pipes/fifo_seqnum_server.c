#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fifo_seqnum.h"

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]) {
  int serverFd, dummyFd, clientFd;
  char clientFifo[CLIENT_FIFO_NAME_LEN];
  struct request req;
  struct response resp;
  int seqNum = 0;  // This is our "service"

  /* Create well-known FIFO, and open it for reading */

  umask(0);  // So we get the permissions we want
  if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
    fprintf(stderr, "mkfifo %s\n", SERVER_FIFO);
    exit(EXIT_FAILURE);
  }
  serverFd = open(SERVER_FIFO, O_RDONLY);
  if (serverFd == -1) {
    fprintf(stderr, "open %s\n", SERVER_FIFO);
    exit(EXIT_FAILURE);
  }

  /* Open an extra write descriptor, so that we never see EOF
     Otherwise, server will not block when no incoming requests
     and keep reading EOF and discarding this empty request */

  dummyFd = open(SERVER_FIFO, O_WRONLY);
  if (dummyFd == -1) {
    fprintf(stderr, "open %s\n", SERVER_FIFO);
    exit(EXIT_FAILURE);
  }

  /* Ignore the SIGPIPE signal, so that if the server attempts to
     write to a client FIFO that doesn't have a reader, then, rather
     than being sent a SIGPIPE signal (which kills a process by
     default), it only receives an EPIPE error from the write()
     system call. */
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    errExit("signal");
  }

  /* Loop that reads and responses to each incoming client request */
  for (;;) {
    if (read(serverFd, &req, sizeof(req)) != sizeof(req)) {
      perror("Error reading request; discarding");
      continue;  // Either partial read or error
    }

    /* Open client FIFO (previously created by client) */

    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)req.pid);
    clientFd = open(clientFifo, O_WRONLY);
    if (clientFd == -1) {  // Open failed, give up on client
      fprintf(stderr, "open %s\n", clientFifo);
      continue;
    }

    /* Send response and close FIFO */

    resp.seqNum = seqNum;
    if (write(clientFd, &resp, sizeof(resp)) != sizeof(resp)) {
      fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
    }
    if (close(clientFd) == -1) {
      perror("close");
    }

    seqNum += req.seqLen;  // Update our sequence number
  }

  return EXIT_SUCCESS;
}
