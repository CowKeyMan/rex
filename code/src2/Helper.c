#include <stdio.h>
#include <stdlib.h>

void error(char *errorMessage){
  perror(errorMessage);
  exit(EXIT_FAILURE);
}