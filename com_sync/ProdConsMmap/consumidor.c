#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_BUFFER              1024            /* tamaño del buffer */
#define DATOS_A_PRODUCIR        100000          /* datos a producir */

sem_t   *huecos;
sem_t   *elementos;
int     *buffer;        /* buffer de numeros enteros */

void consumidor(void);

int main(void) {
  int shd;

  /* se abren los semaforos */
  huecos = sem_open("HUECOS", 0);
  elementos = sem_open("ELEMENTOS", 0);
  if (huecos == SEM_FAILED || elementos == SEM_FAILED) {
    perror("Error en sem_open");
    return 1;
  }

  /* se abre el segmento de memoria compartida utilizado como
          buffer circular */
  shd = shm_open("BUFFER", O_RDONLY, 0700);
  if (shd == -1) {
    perror("Error en shm_open");
    return 1;
  }
  buffer = (int *)mmap(NULL, MAX_BUFFER*sizeof(int), PROT_READ,MAP_SHARED, shd, 0);
  if (buffer == NULL) {
    perror("Error en mmap");
    return 1;
  }

  consumidor();   /* se ejecuta el codigo del consumidor */

  /* se desproyecta el buffer */
  munmap(buffer, MAX_BUFFER*sizeof(int));
  close(shd);

  /* se cierran semÃ¡ros */
  sem_close(huecos);
  sem_close(elementos);
  return 0;
}




/* cÃ³o del proceso productor */
void consumidor(void) {
  int dato;               /* dato a consumir */
  int posicion = 0;       /* posicion que indica el elemento a extraer */
  int j;

  for (j=0; j<DATOS_A_PRODUCIR; j++) {
    dato = j;
    sem_wait(elementos);                    /* un elemento menos */
    dato = buffer[posicion];
    posicion=(posicion+1) % MAX_BUFFER;     /* nueva posicion */
    sem_post(huecos);                       /* un hueco mas */
  }
  return;
}
