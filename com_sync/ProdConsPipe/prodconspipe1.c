#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PRODUCIR  10

struct elemento {
  int a;
  int b;
};

int main(void) {

  struct elemento dato;           /* dato a producir */
  int fildes[2];                  /* tuberia*/
  int ind;

  if (pipe(fildes) < 0) {
    perror("Error al crear la tuberia\n");
    return 1;
  }

  if (fork() == 0) {              /* proceso hijo: productor */
    for (ind=0; ind<PRODUCIR; ind++) {
      /*  produce algun de tipo struct elemento  */
      dato.a=ind;
      dato.b=rand();
      write(fildes[1], (char *) &dato, sizeof(struct elemento));
    }
  }
  else {                        /* proceso padre: consumidor */
    for (;;) {
      read(fildes[0], (char *) &dato, sizeof(struct elemento));
      /* consumir el dato */
      printf("El elemento %d tiene el valor %d\n", dato.a, dato.b);
    }
  }

  return 0;
}

