/* Reading data a line at a time */

#include "read_line.h"

#include <errno.h>
#include <unistd.h>

ssize_t readLine(int fd, void * buffer, size_t n) {
  ssize_t numRead;  // # of bytes fetched by last read()
  size_t totRead;   // Total bytes read so far
  char * buf;
  char ch;

  if (n <= 0 || buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  buf = buffer;  // Cast void * to char *
                 // No pointer arithmetic on "void *"

  totRead = 0;
  for (;;) {
    numRead = read(fd, &ch, 1);

    if (numRead == -1) {
      if (errno == EINTR) {  // Interrupted --> restart read()
        continue;
      }
      else {
        return -1;  // Some other error
      }
    }
    else if (numRead == 0) {  // EOF
      if (totRead == 0) {
        return 0;  // No bytes read; return 0
      }
      else {
        break;  // Some bytes read; add '\0'
      }
    }
    else {                    // 'numRead' must be 1 if we get here
      if (totRead < n - 1) {  // Discard > (n - 1) bytes
        totRead++;
        *buf = ch;
        buf++;
      }

      if (ch == '\n') {
        break;
      }
    }
  }

  *buf = '\0';
  return totRead;
}