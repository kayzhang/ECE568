/* A simple UNIX domain stream socket server */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "us_xfr.h"

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]) {
  struct sockaddr_un addr;
  int sfd;
  ssize_t numRead;
  char buf[BUF_SIZE];

  /* Create a client socket */

  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1) {
    errExit("socket");
  }

  /* Construct server address, and make the connection */
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

  if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    errExit("connect");
  }

  /* Copy stdin to socket */

  while ((numRead = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
    if (write(sfd, buf, numRead) != numRead) {
      errExit("partial/failed write");
    }
  }

  if (numRead == -1) {
    errExit("read");
  }

  /* Closes our socket; server sees EOF */

  exit(EXIT_SUCCESS);
}
