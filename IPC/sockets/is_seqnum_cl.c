/* A client that uses stream sockets */

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "is_seqnum.h"

#define BACKLOG 50

#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]) {
  char * reqLenStr;         // Requested length of sequence
  char seqNumStr[INT_LEN];  // Start of granted sequence
  int cfd;
  ssize_t numRead;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "Usage: %s server-host [sequence-len]\n", argv[0]);
  }

  /* Call getaddrinfo() to obtain a list of addresses
     that we can try connecting to */

  memset(&hints, 0, sizeof(hints));
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  hints.ai_family = AF_UNSPEC;  // Allows IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV;

  if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0) {
    errExit("getaddrinfo");
  }

  /* Walk through returned list until we find an address structure
     than can be used to successfully connect a socket */

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (cfd == -1) {
      continue;  // On error, try next address
    }

    if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1) {
      break;  // Success
    }

    /* Connect failed: close this socket and try next address */

    if (close(cfd) == -1) {
      perror("close");
    }
  }

  if (rp == NULL) {
    errExit("Could not connect socket to any address");
  }

  freeaddrinfo(result);

  /* Send requested sequence length, with terminationg newline */

  reqLenStr = (argc > 2) ? argv[2] : "1";
  if (write(cfd, reqLenStr, strlen(reqLenStr)) != (ssize_t)strlen(reqLenStr)) {
    errExit("Partial/failed write (reqLenstr)");
  }
  if (write(cfd, "\n", 1) != 1) {
    errExit("Partial/failed write (newline)");
  }

  /* Read and display sequence number returned by server */

  numRead = readLine(cfd, seqNumStr, sizeof(seqNumStr));
  if (numRead == -1) {
    errExit("readLine");
  }
  if (numRead == 0) {
    errExit("Unexpected EOF from server");
  }

  printf("Sequence number: %s", seqNumStr);  // Includes '\n'

  exit(EXIT_SUCCESS);  // Closes 'cfd'
}
