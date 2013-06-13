#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_BUFFER              1024            /* tamanio del  buffer */
#define DATOS_A_PRODUCIR        100000          /* datos a producir */

sem_t   *huecos;
sem_t   *elementos;
int     *buffer;                /* puntero al buffer de numeros enteros */

void productor(void);

int main(void) {
  int shd;

  /* se crean e inician  semaforos */
  huecos = sem_open("/HUECOS", O_CREAT, 0700, MAX_BUFFER);
  elementos = sem_open("/ELEMENTOS", O_CREAT, 0700, 0);

  if (huecos == SEM_FAILED || elementos == SEM_FAILED) {
    perror("Error en sem_open");
    return 1;
  }

  /* se crea el segmento de memoria compartida utilizado como buffer circular */
  shd = shm_open("/BUFFER", O_CREAT|O_RDWR, 0700);
  if (shd == -1) {
    perror("Error en shm_open");
    return 1;
  }

  if (ftruncate(shd, MAX_BUFFER*sizeof(int))) {
    perror("Error en ftruncate");
    return 1;
  }

  buffer = (int *)mmap(NULL, MAX_BUFFER*sizeof(int), PROT_WRITE, MAP_SHARED, shd, 0);

  if (buffer == MAP_FAILED ) {
    perror("Error en mmap");
    return 1;
  }

  productor();                            /* se ejecuta el codigo del productor */

  /* se desproyecta el buffer */
  munmap(buffer, MAX_BUFFER*sizeof(int));
  close(shd);
  shm_unlink("BUFFER");


  /* cierran y se destruyen los semáros */
  sem_close(huecos);
  sem_close(elementos);
  sem_unlink("HUECOS");
  sem_unlink("ELEMENTOS");;
  return 0;
}


/* codigo del proceso productor */
void productor(void) {
  int dato;                       /* dato a producir */
  int posicion = 0;               /* posicion donde insertar el elemento*/
  int j;

  for (j=0; j<DATOS_A_PRODUCIR; j++) {
    dato = j;
    sem_wait(huecos);                       /* un hueco menos */
    buffer[posicion]=dato;
    posicion=(posicion+1) % MAX_BUFFER;     /* nueva posicion */
    sem_post(elementos);                    /* un elemento mas */
  }
  return;
}


