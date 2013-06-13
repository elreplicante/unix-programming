#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE  1
#define FALSE 0

volatile int interesado[2]={FALSE,FALSE};
volatile int volatile turno;

volatile int suma_total = 0;
void * suma_parcial(void * arg) {
  int j = 0;
  int tmp;
  int ni=((int*)arg)[0];
  int nf=((int*)arg)[1];
  int yo=((int*)arg)[2];
  int otro=1-yo;

  for (j = ni; j <= nf; j++){

    interesado[yo] = TRUE;
    turno = otro;
    //no hace nada, espera ocupada
    while( interesado[otro]==TRUE && turno==otro );

    //Sección crítica
    tmp=suma_total;
    sched_yield();
    tmp=tmp+j;
    sched_yield();
    suma_total = tmp;
    //Fin de sección crítica

    interesado[yo] = FALSE;
    sched_yield();

  }
  pthread_exit(0);
}

int main(void) {
  pthread_t th1, th2;
  int num1[3]={  1,   4999, 0};
  int num2[3]={5000, 10000, 1};

  /* se crean dos threads con atributos por defecto */
  pthread_create(&th1, NULL, suma_parcial, (void*)num1);
  pthread_create(&th2, NULL, suma_parcial, (void*)num2);

  printf("El thread principal continua ejecutando\n");

  /* se espera su terminacion */
  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  printf("La suma total es: %d y debería ser (50005000)\n", suma_total);

  return 0;
}