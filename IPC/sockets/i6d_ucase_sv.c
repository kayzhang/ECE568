/*IPv6 case-conversion server using datagram sockets */
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
  struct sockaddr_in6 svaddr, claddr;
  int sfd;
  ssize_t numBytes;
  socklen_t len;
  char buf[BUF_SIZE];
  // Maxinum IPv6 hexadecimal string (46-bit)
  char cladrrStr[INET6_ADDRSTRLEN];

  /* Create a server socket */

  sfd = socket(AF_INET6, SOCK_DGRAM, 0);
  if (sfd == -1) {
    errExit("socket");
  }

  /* Construct server address and bind server socket to it */

  memset(&svaddr, 0, sizeof(svaddr));
  svaddr.sin6_family = AF_INET6;
  svaddr.sin6_addr = in6addr_any;      // Wildcard address
                                       // Already network byte order
  svaddr.sin6_port = htons(PORT_NUM);  // host to network short

  if (bind(sfd, (struct sockaddr *)&svaddr, sizeof(svaddr)) == -1) {
    errExit("bind");
  }

  /* Receive messages, convert to uppercase, and return to client */

  for (;;) {
    len = sizeof(struct sockaddr_in6);
    numBytes = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&claddr, &len);
    if (numBytes == -1) {
      errExit("recvfrom");
    }

    // Get the client IPv6 address
    if (inet_ntop(AF_INET6, &claddr.sin6_addr, cladrrStr, sizeof(cladrrStr)) == NULL) {
      printf("Couldn't convert client address to string\n");
    }
    else {
      printf("Server received %ld bytes from (%s, %u)\n",
             (long)numBytes,
             cladrrStr,
             ntohs(claddr.sin6_port));  // network to host short
    }

    for (int i = 0; i < numBytes; i++) {
      buf[i] = toupper((unsigned char)buf[i]);
    }

    if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *)&claddr, len) != numBytes) {
      errExit("sendto");
    }
  }
}
