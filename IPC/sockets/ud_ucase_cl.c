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
  ssize_t msgLen;
  ssize_t numBytes;
  char resp[BUF_SIZE];

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "Usage: %s msg...\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Create client socket; bind to unique pathname (based on PID) */

  sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sfd == -1) {
    errExit("socket");
  }

  memset(&claddr, 0, sizeof(claddr));
  claddr.sun_family = AF_UNIX;
  snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_ucase_cl.%ld", (long)getpid());

  if (bind(sfd, (struct sockaddr *)&claddr, sizeof(claddr)) == -1) {
    errExit("bind");
  }

  /* Construct address for server */

  memset(&svaddr, 0, sizeof(svaddr));
  svaddr.sun_family = AF_UNIX;
  strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

  /* Send messages to server; echo response on stdout */

  for (int i = 1; i < argc; i++) {
    msgLen = strlen(argv[i]);  // May be longer than BUF_SIZE
    if (sendto(sfd, argv[i], msgLen, 0, (struct sockaddr *)&svaddr, sizeof(svaddr)) != msgLen) {
      errExit("sendto");
    }

    numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
    if (numBytes == -1) {
      errExit("recvfrom");
    }
    printf("Response %d: %.*s\n", i, (int)numBytes, resp);
  }

  /* Remove client socket pathname */

  remove(claddr.sun_path);
  exit(EXIT_SUCCESS);
}
