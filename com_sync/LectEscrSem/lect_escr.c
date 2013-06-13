#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int dato = 0;       /* recurso */
int n_lectores = 0; /* numero de lectores */
sem_t sem_lec;      /* controlar el acceso n_lectores */
sem_t cerrojo;      /* controlar el acceso a dato */

void * Lector(void * arg) {
  int i;
  
  for (i=rand()%10;i>0;i--) {
    sem_wait(&sem_lec);
    n_lectores = n_lectores + 1;
    if (n_lectores == 1)
      sem_wait(&cerrojo);
    sem_post(&sem_lec);

    /* leer dato */
    printf("El lector\t%ld lee dato:\t%d\n", pthread_self(), dato);
    
    sem_wait(&sem_lec);
    n_lectores = n_lectores - 1;
    if (n_lectores == 0)
      sem_post(&cerrojo);
    sem_post(&sem_lec);
    
    sleep(rand()%10);
  }
  pthread_exit(0);
}

void * Escritor(void * arg) {
  int i;
  
  for (i=rand()%10;i>0;i--) {
    sem_wait(&cerrojo);

    /* modificar el recurso */
    dato = dato + 2;
    printf("El consumidor\t%ld modifica dato:\t%d\n", pthread_self(), dato);

    sem_post(&cerrojo);

    sleep(rand()%5);
  }
  pthread_exit(0);
}

int main(void) {
  pthread_t th1, th2, th3, th4;
  
  printf("Iniciamos lectores/escritores con dato:\t\t%d\n", dato);
  
  sem_init(&cerrojo, 0, 1);   sem_init(&sem_lec, 0, 1);

  pthread_create(&th1, NULL, Lector, NULL);
  pthread_create(&th2, NULL, Escritor, NULL);
  pthread_create(&th3, NULL, Lector, NULL);
  pthread_create(&th4, NULL, Escritor, NULL);

  pthread_join(th1, NULL);   pthread_join(th2, NULL);
  pthread_join(th3, NULL);   pthread_join(th4, NULL);

  /* cerrar todos los semaforos */
  sem_destroy(&cerrojo);    sem_destroy(&sem_lec);
  printf("Proceso finalizado, dato:\t%d\n", dato);
  exit(0);
}
