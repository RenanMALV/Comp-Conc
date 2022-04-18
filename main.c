/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 1 */
/* Codigo: "Hello World" usando threads em C e a funcao que espera as threads terminarem */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2 //total de threads a serem criadas
#define DATASIZE 10000 // tamanho da coleção de dados

const int dataVector[DATASIZE]; // vetor de dados
int resultVector[DATASIZE]; // vetor dos resultados

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
   int idThread, nThreads;
} t_Args;

//funcao executada pelas threads
void *square (void *arg) {
  t_Args *args = (t_Args *) arg;

  printf("Sou a thread %d de %d threads\n", args->idThread+1, args->nThreads);

  // realiza a divisão dos blocos do vetor
  int blockSize = DATASIZE/args->nThreads;
  int remainder = DATASIZE%args->nThreads;
  int endBound = ((args->idThread + 1) * blockSize);
  int startBound = endBound - blockSize;
  
  if(args->idThread+1 == args->nThreads) //útima thread
    endBound += remainder; // adiciona o resto
  
  //Acessa o vetor de escopo global e realiza as operações
  for(int i = startBound; i<endBound; i++){
    resultVector[i] = dataVector[i]*dataVector[i];
  }

  free(arg); //aqui pode liberar a alocacao feita na main
  
  pthread_exit(NULL);
}

//funcao principal do programa
int main() {
  pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
  int thread; //variavel auxiliar
  t_Args *arg; //receberá os argumentos para a thread

  for(thread=0; thread<NTHREADS; thread++) {
    printf("--Aloca e preenche argumentos para thread %d\n", thread+1);
    arg = malloc(sizeof(t_Args));
    if (arg == NULL) {
      printf("--ERRO: malloc()\n"); exit(-1);
    }
    arg->idThread = thread; 
    arg->nThreads = NTHREADS; 
    
    printf("--Cria a thread %d\n", thread+1);
    if (pthread_create(&tid_sistema[thread], NULL, square, (void*) arg)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  //--espera todas as threads terminarem
  for (thread=0; thread<NTHREADS; thread++) {
    if (pthread_join(tid_sistema[thread], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  }

  printf("--Thread principal terminou\n");
  pthread_exit(NULL);
}