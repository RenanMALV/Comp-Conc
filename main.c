//Verifica o máximo e mínimo entre todos os elementos de um vetor de reais
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

long int dim; //dimensao do vetor de entrada
int nthreads; //numero de threads
double *vetor; //vetor de entrada com dimensao dim 

// strutura que guarda máximo e mínimo
typedef struct{
  double max, min;
} minMax;

//fluxo das threads
void * tarefa(void * arg) {
  long int id = (long int) arg; //identificador da thread
  minMax *valoresLocal; //variavel local dos valores max e min
  // Alocando os dados da variável local na heap
  valoresLocal = (minMax*) malloc(sizeof(minMax));
  if(valoresLocal==NULL) {exit(1);}
  long int tamBloco = dim/nthreads;  //tamanho do bloco de cada thread 
  long int ini = id * tamBloco; //elemento inicial do bloco da thread
  long int fim; //elemento final(nao processado) do bloco da thread
  if(id == nthreads-1) 
   fim = dim;
  else 
   fim = ini + tamBloco; //trata o resto, se houver
  
  //inicializa os valores de max e min do bloco
  valoresLocal->max = vetor[ini];
  valoresLocal->min = vetor[ini];
  //verifica os elementos do bloco da thread
  for(long int i=ini+1; i<fim; i++){
    if(valoresLocal->max < vetor[i])
      valoresLocal->max = vetor[i];
    if(valoresLocal->min > vetor[i])
      valoresLocal->min = vetor[i];
  }
  //retorna o resultado da verificação local (ponteiro)
  pthread_exit((void *) valoresLocal); 
}

void sequencial(){
  minMax valores;
  double ini, fim; //tomada de tempo
  GET_TIME(ini);

  //algoritmo que verifica máximo e mínimo 
  valores.max = vetor[0];
  valores.min = vetor[0];
  for(long int i=1; i<dim; i++){
    if(valores.max < vetor[i])
      valores.max = vetor[i];
    if(valores.min > vetor[i])
      valores.min = vetor[i];
  }
  
  GET_TIME(fim);
  printf("Tempo sequencial:  %lf\n", fim-ini);
  printf("Min seq:  %.12lf | Max seq:  %.12lf\n", valores.min, valores.max);
}

//fluxo principal
int main(int argc, char *argv[]) {
  minMax valores;     // valores globais
  double ini, fim;   //tomada de tempo
  pthread_t *tid;   //identificadores das threads no sistema
  minMax *retorno; //valor de retorno das threads
  srand((unsigned) time(NULL)); // seed para rand
  
  //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
  if(argc < 3) {
     fprintf(stderr, "Digite: %s <dimensao do vetor> <numero threads>\n", argv[0]);
     return 1; 
  }
  dim = atoi(argv[1]);
  nthreads = atoi(argv[2]);
  //aloca o vetor de entrada
  vetor = (double*) malloc(sizeof(double)*dim);
  if(vetor == NULL) {
    fprintf(stderr, "ERRO--malloc\n");
    return 2;
  }
  //preenche o vetor de entrada
  for(long int i=0; i<dim; i++)
    vetor[i] = (double)(rand()%1000000)+(1.0/(double)((rand()%1000)+1.0));
  
  //verificação sequencial dos elementos
  sequencial();
  
  //verificação concorrente dos elementos
  GET_TIME(ini);
  tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if(tid==NULL) {
    fprintf(stderr, "ERRO--malloc\n");
    return 2;
  }
  //criar as threads
  for(long int i=0; i<nthreads; i++) {
    if(pthread_create(tid+i, NULL, tarefa, (void*) i)){
       fprintf(stderr, "ERRO--pthread_create\n");
       return 3;
    }
  }
  // inicializando valores globais
  valores.max = vetor[0];
  valores.min = vetor[0];
  //aguardar o termino das threads
  for(long int i=0; i<nthreads; i++) {
    if(pthread_join(*(tid+i), (void**) &retorno)){
      fprintf(stderr, "ERRO--pthread_join\n");
      return 3;
    }

    // verificando max e min global
    if(valores.max < retorno->max)
      valores.max = retorno->max;
    if(valores.min > retorno->min)
      valores.min = retorno->min;
    
    // liberando o espaço de memória criado dentro das threads
    free(retorno);
  }
  GET_TIME(fim);
  printf("Tempo concorrente:  %lf\n", fim-ini);
  
  printf("Min conc:  %.12lf | Max conc:  %.12lf\n", valores.min, valores.max);
  
  //libera as areas de memoria alocadas
  free(vetor);
  free(tid);
  
  return 0;
}