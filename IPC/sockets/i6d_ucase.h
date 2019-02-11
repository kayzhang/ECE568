/* Header file used by i6d_ucase_sv.c and i6d_ucase_cl.c */

#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <sys/socket.h>

/* Maximum size of messages exchanged
   between client to server */
#define BUF_SIZE 10

#define PORT_NUM 50002  // Server port number
