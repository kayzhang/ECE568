/* Header file used by ud_ucase_sv.c and ud_ucase_sl.c */

#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>

/* Maximum size of messages exchanged
   between client to server */
#define BUF_SIZE 10

#define SV_SOCK_PATH "/tmp/ud_ucase"
