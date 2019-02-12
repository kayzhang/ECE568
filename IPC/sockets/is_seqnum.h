/* Header file used by is_seqnum_sv.c and is_seqnum_cl.c */

#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>

#include "read_line.h"    // Declaration of readLine()
#define PORT_NUM "50000"  // Port number for server

// Size of string able to hold largest
// integer (including terminating '\n')
#define INT_LEN 30
