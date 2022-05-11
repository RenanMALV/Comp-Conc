/*Lab-4
  Processa primos*/
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

int *entrada; //matriz de entrada
float *saida; //matriz de saida
float *result; // matriz para verificar a corretude
int nthreads; //numero de threads

int global_iter; //iterador global
pthread_mutex_t mutex; //variavel de lock para exclusao mutua

typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int dim; //dimensao das estruturas de entrada
} tArgs;

int ehPrimo(int n){
  if(n <= 1)
    return 0;
  if(n == 2)
    return 1;
  if(n%2 == 0)
    return 0;
  for(int i = 3; i < sqrt(n)+1; i+=2){
    if(n%i == 0)
      return 0;
  }
  return 1;
}

//funcao que as threads executarao
void * tarefa(void *arg) {
   tArgs *args = (tArgs*) arg;
   //printf("Thread %d\n", args->id);
  
  // inpicio do trecho crítico com mutex
  pthread_mutex_lock(&mutex);
  int i = global_iter;
  global_iter++;
  pthread_mutex_unlock(&mutex);
  // fim do trecho crítico
  while(i<args->dim){
    if (ehPrimo(entrada[i]))
      saida[i] = sqrt(entrada[i]);
    else
      saida[i] = entrada[i];

    // inpicio do trecho crítico com mutex
    pthread_mutex_lock(&mutex);
    i = global_iter;
    global_iter++;
    pthread_mutex_unlock(&mutex);
    // fim do trecho crítico
  }
  
  pthread_exit(NULL);
  //return NULL;
}

//fluxo principal
int main(int argc, char* argv[]) {
   int dim; //dimensao da matriz de entrada
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e dimensao
   double inicio, fim, delta;
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   if(argc<3) {
      printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
      return 1;
   }
   dim = atoi(argv[1]);
   nthreads = atoi(argv[2]);
   if (nthreads > dim) nthreads=dim;

   //alocacao de memoria para as estruturas de dados
   entrada = (int *) malloc(sizeof(int) * dim);
   if (entrada == NULL) {printf("ERRO--malloc\n"); return 2;}
   saida = (float *) malloc(sizeof(float) * dim);
   if (saida == NULL) {printf("ERRO--malloc\n"); return 2;}
   result = (float *) malloc(sizeof(float) * dim * dim);
   if (result == NULL) {printf("ERRO--malloc\n"); return 2;}

   srand((unsigned) time(NULL));
   //inicializacao das estruturas de dados de entrada e saida
   for(int i=0; i<dim; i++) {
    entrada[i] = rand()%1000; 
    saida[i] = 0;
    result[i] = 0;
   }

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);
  
   GET_TIME(fim);
   delta = fim - inicio;
   //printf("Tempo inicializacao:%lf\n", delta);
  
   //início da tarefa
   GET_TIME(inicio);
   //alocacao das estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}
   args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
   if(args==NULL) {puts("ERRO--malloc"); return 2;}
   //criacao das threads
   for(int i=0; i<nthreads; i++) {
      (args+i)->id = i;
      (args+i)->dim = dim;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   } 
   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }
   GET_TIME(fim);   
   delta = fim - inicio;
   printf("Tempo concorrente: %lf\n", delta);

  //calculando o resultado sequencial
  GET_TIME(inicio);
  for(int i=0; i<dim; i++) {
    if (ehPrimo(entrada[i]))
      result[i] = sqrt(entrada[i]);
    else
      result[i] = entrada[i];
  }
  GET_TIME(fim);   
  delta = fim - inicio;
  printf("Tempo sequencial: %lf\n", delta);
  printf("%lf %lf\n", saida[0], result[0]);
  for(int i=0; i<dim; i++) {
    if(result[i] != saida[i]){
      printf("Falhou no teste de corretude\n");
      free(entrada);
      free(saida);
      free(result);
      free(args);
      free(tid);
      pthread_mutex_destroy(&mutex);
      exit(-1);
    }
  }
  //liberacao da memoria
  GET_TIME(inicio);
  free(entrada);
  free(saida);
  free(result);
  free(args);
  free(tid);
  pthread_mutex_destroy(&mutex);
  GET_TIME(fim);   
  delta = fim - inicio;
  //printf("Tempo finalizacao:%lf\n", delta);

  printf("Passou no teste de corretude\n");

  return 0;
}