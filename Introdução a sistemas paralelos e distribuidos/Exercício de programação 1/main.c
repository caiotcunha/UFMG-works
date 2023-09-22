#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "spend_time.h"

// pythreads

// Abstract Data Type to represent resources
typedef struct{
    int free[8];
} Resources;
// global resources
Resources *resources;

// Abstract Data Type to represent threads


//void* start_routine(void *arg);
//int pthread_create(pthread_t *thread, pthread_attr_t *attr,
//void *(*start_routine)(void *), void *arg);
//int pthread_join(pthread_t thread, void **value_ptr);
//pthread_t pthread_self(void);
//void pthread_exit(void * value_ptr);



//pthread_mutex_t mutex;
//pthread_mutex_init( &mutex, NULL /* atributos */ );
//pthread_mutex_lock( &mutex );
// seção crítica
//pthread_mutex_unlock( &mutex );




//thread_cond_t condition;
//pthread_cond_init ( &condition, &attr );
//pthread_cond_destroy ( &condition);
//pthread_cond_wait ( &condition, &mutex );
//pthread_cond_signal ( &condition ); /* sinaliza um */

// 8 recursos

// mecanismo quando chamado deve permitir que 
// um thread indique quais dos 8 ela precisa
// e só prossegue quando ela pega todos

// max 1000 threads
// todas identificadores diferentes

void printArray(int *array, int size){
    for(int i = 0; i < size; i++){
        printf(" %d", array[i]);
    }
}

//void init_recursos(void) - inicializa a estrutura de dados que você definir para representar os recursos;
//void trava_recursos(recursos) - uma thread pede para reservar um conjunto de recursos, representados da forma que seu programa preferir;
//void libera_recursos(void) - uma thread que já reservou alguns recursos sinaliza que completou seu trabalho crítico, liberando os recursos.

int main(void){

    //inicialize resources all free
    for(int i = 0; i<8;i++){
        resources->free[i] = 1;
    }

    //leitura da linha
    int identifier = 0;
    int freeTime = 0;
    int criticalTime = 0;
    int listOfResources[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    while(scanf(" %d", &identifier) == 1)  {
        scanf(" %d", &freeTime );
        scanf(" %d", &criticalTime);

        printf("%d %d %d\n", identifier, freeTime, criticalTime);

        for(int i = 0; i < 8; i++){
            char resource;
            resource = getchar();
            if(resource == ' '){
                resource = getchar();
            };
            if(resource == '\n'){
                printf("caiu");
                break;
            }
            listOfResources[i] = atoi(&resource);
        }

        //Cada thread deve ser criada assim que a linha com sua descrição seja lida.


    };

}















// No início da thread, ela deve receber como parâmetros seu tid, os tempos livre e crítico
//    e os recursos que ela necessita para completar seu trabalho
// Em segunda, ela deve executar as operações:
//   spend_time(tid,NULL,tlivre);
//   trava_recursos(recursos);     // a forma de representar os recursos é uma decisão do desenvolvedor
//   spend_time(tid,"C",tcritico);
//   libera_recursos();            // note que cada thread deve ser ter sua lista de recursos registrada em algum lugar
//   pthread_exit(); 