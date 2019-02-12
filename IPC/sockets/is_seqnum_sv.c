/* An iterative server that uses a stream
   socket to communicate with clients */

#define _BSD_SOURCE  // To get definitions of NI_MAXHOST and \
                     // NI_MAXSERV from <netdb.h>

#include <netdb.h>
#include <signal.h>
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
  uint32_t seqNum;
  char reqLenStr[INT_LEN];  // Length of requested sequence
  char seqNumStr[INT_LEN];  // Start of granted sequence
  struct sockaddr_storage claddr;
  int lfd, cfd, optval, reqLen;
  socklen_t addrlen;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  char addrStr[ADDRSTRLEN];
  char host[NI_MAXHOST];
  char service[NI_MAXSERV];

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "%s [init-seq-num]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  seqNum = (argc > 1) ? atoi(argv[1]) : 0;

  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    errExit("signal");
  }

  /* Call getaddrinfo() to obtain a list of addresses that
     we can try binding to */

  memset(&hints, 0, sizeof(hints));
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;  // Allows IPv4 or IPv6
  // Wildcard IP address; service name is numeric
  hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
  if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0) {
    errExit("getaddrinfo");
  }

  /* Walk through returned list until we find an address structure
     than can be used to successfully create and bind a socket */

  optval = 1;
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (lfd == -1) {
      continue;  // On error, try next address
    }

    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
      errExit("setsockopt");
    }

    if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0) {
      break;  // Success
    }

    /* bind() failed: close this socket and try next address */

    if (close(lfd) == -1) {
      perror("close");
    }
  }

  if (rp == NULL) {
    errExit("Could not bind socket to any address");
  }

  if (listen(lfd, BACKLOG) == -1) {
    errExit("listen");
  }

  freeaddrinfo(result);

  for (;;) {  // Handle clients iteratively
    /* Accept a client connection, obtaining client's address */

    addrlen = sizeof(struct sockaddr_storage);
    cfd = accept(lfd, (struct sockaddr *)&claddr, &addrlen);
    if (cfd == -1) {
      perror("accept");
      continue;
    }

    if (getnameinfo(
            (struct sockaddr *)&claddr, addrlen, host, sizeof(host), service, sizeof(service), 0) ==
        0) {
      snprintf(addrStr, sizeof(addrStr), "(%s, %s)", host, service);
    }
    else {
      snprintf(addrStr, sizeof(addrStr), "(?UNKNOWN)?");
    }
    printf("Connection from %s\n", addrStr);

    /* Read client request, send sequence number back */

    if (readLine(cfd, reqLenStr, sizeof(reqLenStr)) <= 0) {
      close(cfd);
      continue;  // Failed read; skip request
    }

    reqLen = atoi(reqLenStr);
    if (reqLen <= 0) {  // Watch for misbehaving clients
      close(cfd);
      continue;  // Bad request; skip it
    }

    snprintf(seqNumStr, sizeof(seqNumStr), "%d\n", seqNum);
    if (write(cfd, &seqNumStr, strlen(seqNumStr)) != (ssize_t)strlen(seqNumStr)) {
      fprintf(stderr, "Error on write");
    }

    seqNum += reqLen;  // Update sequence number

    if (close(cfd) == -1) {
      perror("close");  // Close connection
    }
  }
}
