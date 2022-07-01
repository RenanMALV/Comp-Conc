/*
Lab-9
Disciplina: Computacao Concorrente 
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define NTHREADS  5

typedef struct{
  int id; //identificador do elemento que a thread ira processar
} tArgs;

/* semáforos */
sem_t meio_cond;
sem_t ultimo_cond;
sem_t semBin;
// contador para o último semáforo
int ultimo_contador=0;

/* Thread 5 */
void * inicio() {
  printf("Seja bem-vindo!\n");

  // desbloqueia 3 threads do tipo meio
  for(int i=0;i<3;i++)
    sem_post(&meio_cond);

  pthread_exit(NULL);
}

/* Thread 2, 3 e 4 */
void * meio(void *arg) {
  tArgs *args = (tArgs*) arg;

  // espera o semáforo do tipo meio
  sem_wait(&meio_cond);

  if(args->id == 1)
    printf("Fique a vontade.\n");
  else if(args->id == 2)
    printf("Sente-se por favor.\n");
  else
    printf("Aceita um copo d’agua?.\n");

  // garante exclusão mútua
  sem_wait(&semBin);
  // soma no contador que dispara a última thread 
  ultimo_contador++;
  // fim da seção crítica
  sem_post(&semBin);

  // verifica se é a última thread do tipo meio
  if (ultimo_contador == 3)
    // libera uma thread do tipo fim
    sem_post(&ultimo_cond);
  
  pthread_exit(NULL);
}

/* Thread 1 */
void * fim() {

  // espera o ultimo semáforo
  sem_wait(&ultimo_cond);
  
  printf("Volte sempre!\n");
  pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
  int i; 
  pthread_t threads[NTHREADS];

  /* Inicilaiza os semáforos */
  // threads do meio
  sem_init(&meio_cond, 0, 0);
  //thread do fim
  sem_init(&ultimo_cond, 0, 0);
  // semáforo binário para exclusão mútua
  sem_init(&semBin, 0, 1);

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

  /* Desaloca variaveis e semáforos terminando o programa */
  free(args);
  sem_destroy(&meio_cond);
  sem_destroy(&ultimo_cond);  
  sem_destroy(&semBin);
  pthread_exit(NULL);
}