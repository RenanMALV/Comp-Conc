/*
Lab-5
Disciplina: Computacao Concorrente 
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  5

typedef struct{
  int id; //identificador do elemento que a thread ira processar
} tArgs;

/* Variaveis globais */
int primeiro_contador = 0;
int ultimo_contador = 0;
pthread_mutex_t primeiro_contador_mutex;
pthread_mutex_t ultimo_contador_mutex;
pthread_cond_t meio_cond;
pthread_cond_t ultimo_cond;


/* Thread 5 */
void * inicio() {
  printf("Seja bem-vindo!\n");

  pthread_mutex_lock(&primeiro_contador_mutex);
  primeiro_contador++;
  pthread_mutex_unlock(&primeiro_contador_mutex);

  pthread_mutex_lock(&ultimo_contador_mutex);
  pthread_cond_broadcast(&meio_cond);
  pthread_mutex_unlock(&ultimo_contador_mutex);

  pthread_exit(NULL);
}

/* Thread 2, 3 e 4 */
void * meio(void *arg) {
  tArgs *args = (tArgs*) arg;
  pthread_mutex_lock(&primeiro_contador_mutex);
  if (primeiro_contador == 0){
      pthread_cond_wait(&meio_cond, &primeiro_contador_mutex);
  }  
  pthread_mutex_unlock(&primeiro_contador_mutex);

  if(args->id == 1)
    printf("Fique a vontade.\n");
  else if(args->id == 2)
    printf("Sente-se por favor.\n");
  else
    printf("Aceita um copo d’agua?.\n");
  
  
  pthread_mutex_lock(&ultimo_contador_mutex);
  ultimo_contador++;
  if (ultimo_contador == 3) { 
      pthread_cond_signal(&ultimo_cond);
  }
  pthread_mutex_unlock(&ultimo_contador_mutex); 
  
  pthread_exit(NULL);
}

/* Thread 1 */
void * fim() {
  pthread_mutex_lock(&ultimo_contador_mutex);
  if (ultimo_contador < 3) { 
     pthread_cond_wait(&ultimo_cond, &ultimo_contador_mutex);
  }
  pthread_mutex_unlock(&ultimo_contador_mutex); 
  printf("Volte sempre!\n");
  pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
  int i; 
  pthread_t threads[NTHREADS];

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&primeiro_contador_mutex, NULL);
  pthread_mutex_init(&ultimo_contador_mutex, NULL);
  pthread_cond_init (&meio_cond, NULL);
  pthread_cond_init (&ultimo_cond, NULL);

  tArgs *args = (tArgs*) malloc(sizeof(tArgs)*3);
  if(args==NULL) {puts("ERRO--malloc"); return 2;}
   

  /* Cria as threads */
  pthread_create(&threads[0], NULL, fim, NULL);
  for(int i=1; i <= 3; i++){
    (args+(i-1))->id = i;
    pthread_create(&threads[i], NULL, meio, (void*) (args+(i-1))); 
  }
  pthread_create(&threads[4], NULL, inicio, NULL);

  /* Espera todas as threads completarem */
  for (i = 0; i < NTHREADS; i++) {
  pthread_join(threads[i], NULL);
  }

  /* Desaloca variaveis e termina */
  free(args);
  pthread_mutex_destroy(&primeiro_contador_mutex);
  pthread_mutex_destroy(&ultimo_contador_mutex);
  pthread_cond_destroy(&meio_cond);
  pthread_cond_destroy(&ultimo_cond);
}