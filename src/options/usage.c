#include <stdio.h>
#include <stdlib.h>

void
usage (int status)
{
  if (status == EXIT_FAILURE)
    printf("Try ttar -h or ttar --help for more informations.\n");
  else
    printf("ttar : [OPTIONS] <archive_name> ...\n");
  exit(status);
}
