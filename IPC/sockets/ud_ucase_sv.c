/* A simple UNIX domain datagram socket server */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ud_ucase.h"

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]) {
  struct sockaddr_un svaddr, claddr;
  int sfd;
  ssize_t numBytes;
  socklen_t len;
  char buf[BUF_SIZE];

  /* Create a server socket */

  sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sfd == -1) {
    errExit("socket");
  }

  /* Construct well-known address and bind server socket to it */
  if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
    fprintf(stderr, "remove-%s", SV_SOCK_PATH);
    exit(EXIT_FAILURE);
  }

  memset(&svaddr, 0, sizeof(svaddr));
  svaddr.sun_family = AF_UNIX;
  strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *)&svaddr, sizeof(svaddr)) == -1) {
    errExit("bind");
  }

  /* Receive messages, convert to uppercase, and return to client */

  for (;;) {
    len = sizeof(struct sockaddr_un);
    numBytes = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&claddr, &len);
    if (numBytes == -1) {
      errExit("recvfrom");
    }

    printf("Server received %ld bytes from %s\n", (long)numBytes, claddr.sun_path);

    for (int i = 0; i < numBytes; i++) {
      buf[i] = toupper((unsigned char)buf[i]);
    }

    if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *)&claddr, len) != numBytes) {
      errExit("sendto");
    }
  }
}
