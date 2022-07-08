/*
Lab-10
Disciplina: Computacao Concorrente 
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h> // função sleep 

//leitores
#define L 5
//escritores
#define E 5 

int cont_l=0; //contador de leitores
int cont_e=0; //contador de escritores

sem_t mutex_l, mutex_e; // mutex
sem_t sem_l, sem_e; // semaforo de leitores e escritores


// Thread Leitora
void * leitor (void * arg) {
	int *id = (int *) arg;
	while(1) {
		printf("Leitor %d quer ler\n", *id);
		
		sem_wait(&sem_l); // garante prioridade para escritores
		
		printf("Leitor %d pode ler\n", *id);
		
		sem_wait(&mutex_l);
		cont_l++;
		if(cont_l==1){
			sem_wait(&sem_e);
			printf("Leitor %d bloqueando escrita\n", *id);
		}
		sem_post(&mutex_l);                      
		sem_post(&sem_l);
	
		printf("Leitor %d lendo\n", *id);
		
		sleep(1);  //realiza leitura
		
		printf("Leitor %d terminou a leitura\n", *id);
	
		sem_wait(&mutex_l); 
		cont_l--;
		if(cont_l==0){
			printf("Leitor %d desbloqueando escrita\n", *id);
			sem_post(&sem_e);
		}
		sem_post(&mutex_l);
		
	}
	free(arg);
	pthread_exit(NULL);
}

// Thread Escritora
void * escritor (void * arg) {
	int *id = (int *) arg;
	while(1) {
		printf("Escritor %d quer escrever\n", *id);
		
		sem_wait(&mutex_e); 
		cont_e++;
		if(cont_e==1){
			sem_wait(&sem_l);
			printf("Escritor %d bloqueando novos leitores\n", *id);
		}
		sem_post(&mutex_e);
		sem_wait(&sem_e);
		
		printf("Escritor %d escrevendo\n", *id);                  
		//realiza escrita
		sleep(1);  // simula escrita
	
		printf("Escritor %d terminou a escrita\n", *id);
		
		sem_post(&sem_e);
		sem_wait(&mutex_e);
		cont_e--;
		if(cont_e==0){
			printf("Escritor %d desbloqueando leitores\n", *id);
			sem_post(&sem_l);
		}
		sem_post(&mutex_e);

		sleep(5); // simula o tempo de chegada de um novo escritor
	}
	free(arg);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int i; 
    pthread_t tid[L+E];  //identificadores das threads no sistema
    int id[L+E];  //identificadores locais das threads

    // Inicia os semáforos binários
    sem_init(&mutex_l, 0, 1);
    sem_init(&mutex_e, 0, 1);
    sem_init(&sem_l, 0, 1);      
    sem_init(&sem_e, 0, 1);      

    //cria leitores
    for(int i=0; i<L; i++) {
        id[i] = i+1;
        if(pthread_create(&tid[i], NULL, leitor, (void *) &id[i]))
          exit(-1);
    } 

    //cria escritores
    for(int i=0; i<E; i++) {
        id[i+L] = i+1;
        if(pthread_create(&tid[i+L], NULL, escritor, (void *) &id[i+L]))
          exit(-1);
    } 

    /* Espera todas as threads completarem */
    for (i = 0; i < L+E; i++) {
        pthread_join(tid[i], NULL);
    }

    /* Desaloca variaveis e termina */
    sem_destroy(&mutex_l);
    sem_destroy(&mutex_e);
    sem_destroy(&sem_l);
    sem_destroy(&sem_e);

    pthread_exit(NULL);
}