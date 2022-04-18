/* Disciplina: Computacao Concorrente */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <time.h>

#define NTHREADS 2 //total de threads a serem criadas
#define DATASIZE 10000 // tamanho da coleção de dados

int dataVector[DATASIZE]; // vetor de dados
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
  printf("--computou o bloco de intervalo [%d,%d]\n",startBound,endBound-1);

  free(arg); //aqui pode liberar a alocacao feita na main
  
  pthread_exit(NULL);
}

//funcao principal do programa
int main() {

  // inicializa o vetor de dados
  srand((unsigned) time(NULL));
  for(int i = 0; i<DATASIZE; i++){
    dataVector[i] = rand()%1000;
  }
  
  pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
  int thread; //variavel auxiliar
  t_Args *arg; //receberá os argumentos para a thread

  //inicia a contagem de tempo
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  //cria as threads
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
  // termina a contagem de tempo após a função de sincronização bloqueante
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  unsigned int delta_s = (end.tv_sec - start.tv_sec);
  long int delta_ns = (end.tv_nsec - start.tv_nsec);
  if((end.tv_nsec - start.tv_nsec)<0){
    delta_ns += 1000000000;
    delta_s--;
  }
  printf("\nO processamento concorrente demorou %d.%.9ld segundos.\n", delta_s, delta_ns);

  // Verifica a corretude do código
  for(int i = 0; i<DATASIZE; i++){
    if(resultVector[i] != dataVector[i]*dataVector[i]){
      printf("\n falhou ao verificar corretude.");
      exit(-1);
    }
  }
  printf("\nverificou corretude com sucesso.\n");
  
  printf("--Thread principal terminou\n");
  pthread_exit(NULL);
}