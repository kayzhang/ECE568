/* The Linux Abstract Socket Namespace */
/* Only work for Linux */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

void errExit(char * str) {
  perror(str);
  exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]) {
  int sockfd;
  struct sockaddr_un addr;

  memset(&addr, 0, sizeof(struct sockaddr_un)); /* Clear address structure */
  addr.sun_family = AF_UNIX;                    /* UNIX domain address */

  /* addr.sun_path[0] has already been set to 0 by memset() */

  /* Abstract name is "xyz" followed by null bytes */
  strncpy(&addr.sun_path[1], "xyz", sizeof(addr.sun_path) - 2);

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd == -1)
    errExit("socket");

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    errExit("bind");

  sleep(60);

  exit(EXIT_SUCCESS);
}
