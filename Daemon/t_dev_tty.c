#include <stdio.h>
#include <stdlib.h>

int main(void) {
  fprintf(stdout, "This should be printed to standard output\nwhich can be redirected.\n");

  FILE * tty = fopen("/dev/tty", "w");
  fprintf(tty,
          "This should be printed to the controlling tty\nno matter if the standard output is "
          "redirected.\n");
  fclose(tty);

  return EXIT_SUCCESS;
}
