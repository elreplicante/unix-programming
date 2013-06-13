#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_BUFFER          3        //tamañio del buffer
#define DATOS_A_PRODUCIR   10        //datos a producir
pthread_mutex_t mutex;               //mutex para buffer compartido
pthread_cond_t evento;               //evento de producción/consumición
int n_elementos=0;                   //numero de elementos en el buffer
int buffer[MAX_BUFFER];              //buffer comun
int indProd = 0;                     //Indice de producción
int indCons = 0;                     //indice de consumicion
int n_prod  = 0;                     //elementos producidos hasta la fecha

void * Productor(void * arg) {
  int dato;

  while (1) {

    dato = rand()%100;                //producir dato aleatorio entre 0 y 99
    sleep(rand()%3);                  //Esperamos un ratito (entre 0 y 2s) para simular producción
    pthread_mutex_lock(&mutex);       //acceder al buffer

    //esperar hasta que haya
    while (n_elementos == MAX_BUFFER && n_prod!=DATOS_A_PRODUCIR)
      pthread_cond_wait(&evento, &mutex);

    //¿Hemos terminado con la producción?
    if (n_prod==DATOS_A_PRODUCIR) {
      //Enviamos evento y salimos
      //pthread_cond_broadcast(&evento);
      pthread_mutex_unlock(&mutex);
      break;
    }

    //Insertar elemento en sección crítica
    printf("Producimos elemento %d=%d insertado en posición %d\n", n_prod, dato, indProd);
    buffer[indProd] = dato;
    indProd = (indProd + 1) % MAX_BUFFER;
    n_elementos ++;
    n_prod++;
    pthread_cond_broadcast(&evento);     //buffer no vacío
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(0);
}

void * Consumidor(void * arg) {
  int dato;
  
  while (1) {
    pthread_mutex_lock(&mutex);    //acceder al buffer */

    //si buffer vacio y falta por producir
    while (n_elementos == 0 && n_prod!=DATOS_A_PRODUCIR)
      pthread_cond_wait(&evento, &mutex);

    //¿Hemos terminado con la producción?
    if (n_prod==DATOS_A_PRODUCIR) {
      //Enviamos evento y salimos
      //pthread_cond_broadcast(&evento);
      pthread_mutex_unlock(&mutex);
      break;
    }
    
    //Extraer elemento en sección crítica    
    dato = buffer[indCons];
    printf("Consumimos elemento %d extraido de posición %d\n", dato, indCons);
    indCons = (indCons + 1) % MAX_BUFFER;
    n_elementos--;
    pthread_cond_broadcast(&evento);   //buffer no lleno
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(0);
}

int main(int argc, char *argv[]) {
  pthread_t pTh1, pTh2;
  pthread_t cTh1, cTh2;

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&evento, NULL);

  pthread_create(&pTh1, NULL, Productor, NULL);
  pthread_create(&pTh2, NULL, Productor, NULL);

  pthread_create(&cTh1, NULL, Consumidor, NULL);
  pthread_create(&cTh2, NULL, Consumidor, NULL);


  pthread_join(pTh1, NULL);
  pthread_join(pTh2, NULL);
  pthread_join(cTh1, NULL);
  pthread_join(cTh2, NULL);

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&evento);
  exit(0);
}


