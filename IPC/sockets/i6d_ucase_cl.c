/* IPv6 case-conversion client using datagram sockets */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "i6d_ucase.h"

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]) {
  struct sockaddr_in6 svaddr;
  int sfd;
  ssize_t msgLen;
  ssize_t numBytes;
  char resp[BUF_SIZE];

  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "Usage: %s host-address msg...\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Create client socket */
  /* Unlike UNIX domain sockets, if an Internet domain socket is
     not bound to an address, the kernel binds the socket to an
     ephemeral port on the host system */

  sfd = socket(AF_INET6, SOCK_DGRAM, 0);
  if (sfd == -1) {
    errExit("socket");
  }

  /* Construct address for server */

  memset(&svaddr, 0, sizeof(svaddr));
  svaddr.sin6_family = AF_INET6;
  svaddr.sin6_port = htons(PORT_NUM);
  if (inet_pton(AF_INET6, argv[1], &svaddr.sin6_addr) <= 0) {
    fprintf(stderr, "inet_pton failed for address '%s'", argv[1]);
    exit(EXIT_FAILURE);
  }

  /* Send messages to server; echo response on stdout */

  for (int i = 2; i < argc; i++) {
    msgLen = strlen(argv[i]);  // May be longer than BUF_SIZE
    if (sendto(sfd, argv[i], msgLen, 0, (struct sockaddr *)&svaddr, sizeof(svaddr)) != msgLen) {
      errExit("sendto");
    }

    numBytes = recvfrom(sfd, resp, sizeof(resp), 0, NULL, NULL);
    if (numBytes == -1) {
      errExit("recvfrom");
    }
    printf("Response %d: %.*s\n", i, (int)numBytes, resp);
  }

  exit(EXIT_SUCCESS);
}
