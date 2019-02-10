/* A simple UNIX domain stream socket server */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "us_xfr.h"

#define BACKLOG 5

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]) {
  struct sockaddr_un addr;
  int sfd, cfd;
  ssize_t numRead;
  char buf[BUF_SIZE];

  /* Create a server socket */

  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1) {
    errExit("socket");
  }

  /* Construct server socket address, bind socket to it,
     and make this a listening socket */

  if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
    fprintf(stderr, "remove-%s\n", SV_SOCK_PATH);
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    errExit("bind");
  }

  if (listen(sfd, BACKLOG) == -1) {
    errExit("listen");
  }

  /* Handle client connections iteratively */

  for (;;) {
    /* Accept a connection. The connection is returned on a new
       socket, 'cfd'; the listening socket ('sfd') remains open
       and can be used to accept further connections. */

    cfd = accept(sfd, NULL, NULL);
    if (cfd == -1) {
      errExit("accept");
    }

    /* Transfer data from connected socket to stdout until EOF */

    while ((numRead = read(cfd, &buf, sizeof(buf))) > 0) {
      if (write(STDOUT_FILENO, buf, numRead) != numRead) {
        errExit("partial/failed write");
      }
    }

    if (numRead == -1) {
      errExit("read");
    }

    if (close(cfd == -1)) {
      perror("close");
    }
  }
}
