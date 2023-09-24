#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "spend_time.h"

// pythreads

// tipo abstrato de dados para representar recursos
typedef struct{
    int livres[8];
} Resources;
// global resources
Resources *resources;

// tipo abstrato de dados para representar threads
typedef struct{
    int identificador;
    int tempo_livre;
    int tempo_critico;
    int lista_de_recursos[8];
} Thread;

int contador_de_threads = 0;
pthread_t *threads[1000];

//mutex para proteger a sessão cŕitica
pthread_mutex_t mutex;
//condição para testar se todos os recursos estão livres
pthread_cond_t recursos_livres;


//void* start_routine(void *arg);
//int pthread_create(pthread_t *thread, pthread_attr_t *attr,
//void *(*start_routine)(void *), void *arg);
//int pthread_join(pthread_t thread, void **value_ptr);
//pthread_t pthread_self(void);
//void pthread_exit(void * value_ptr);



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

void escreve_array(int *array, int size){
    for(int i = 0; i < size; i++){
        printf(" %d", array[i]);
    }
}

//função para testar se todos os recursos estão livres
int testa_recursos_livres(int *array){
    for(int i = 0; i < 8; i++){
        if(array[i] != -1){
            if(resources->livres[array[i]] != 1){
                return 0;
            }
        }
    }
    return 1;
}

void init_recursos(void){
    resources = malloc(sizeof(Resources));
    //inicializa todos os recursos como livres
    for(int i = 0; i<8;i++){
        resources->livres[i] = 1;
    }
}
void trava_recursos(int *recursos){
    for(int i = 0; i < 8; i++){
        if(recursos[i] != -1){
            resources->livres[recursos[i]] = 0;
        }
    }
}

void libera_recursos(int *recursos){
// uma thread que já reservou alguns recursos sinaliza que completou seu trabalho crítico,
// liberando os recursos.
    for(int i = 0; i < 8; i++){
        if(recursos[i] != -1){
            resources->livres[recursos[i]] = 1;
        }
    }
}

void *rotina_inicial(void *data){
    Thread *teste = (Thread*)data;
    int tid = teste->identificador;
    int tlivre = teste->tempo_livre;
    int tcritico = teste->tempo_critico;
    int *lista_de_recursos = teste->lista_de_recursos;

    spend_time(tid,NULL,tlivre);
    pthread_mutex_lock( &mutex );

    //sessão crítica
    while(!testa_recursos_livres(lista_de_recursos)){
        pthread_cond_wait ( &recursos_livres, &mutex );
    }
    //o acesso aos recursos deve ser feito apenas por uma thread para evitar
    //condição de corrida
    trava_recursos(lista_de_recursos);
    pthread_mutex_unlock( &mutex );
    spend_time(tid,"C",tcritico);
    libera_recursos(lista_de_recursos);
    pthread_cond_signal ( &recursos_livres );
    pthread_exit(NULL);
}


int main(void){
    pthread_mutex_init( &mutex, NULL);
    pthread_cond_init ( &recursos_livres, NULL );
    init_recursos();

    //leitura da linha
    int identificador = 0;
    int tempo_livre = 0;
    int tempo_critico = 0;

    while(scanf(" %d", &identificador) == 1)  {
        int lista_de_recursos[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
        scanf(" %d", &tempo_livre );
        scanf(" %d", &tempo_critico);

        for(int i = 0; i < 8; i++){
            char resource;
            resource = getchar();
            if(resource == ' '){
                resource = getchar();
            };
            if(resource == '\n'){
                break;
            }
            lista_de_recursos[i] = atoi(&resource);
        }

        // Cria e inicializa estrutura abstrata da thread
        Thread *thread = malloc(sizeof(Thread));
        thread->identificador = identificador;
        thread->tempo_livre = tempo_livre;
        thread->tempo_critico = tempo_critico;
        for(int i = 0; i < 8; i++){
            thread->lista_de_recursos[i] = lista_de_recursos[i];
        }

        // Cada thread deve ser criada assim que a linha com sua descrição seja lida.
        pthread_t *th1 = malloc(sizeof(pthread_t));;
        contador_de_threads++;
        threads[contador_de_threads] = th1;
        pthread_create(th1, NULL, rotina_inicial,thread);

    };

    for(int i = 0; i < contador_de_threads; i++){
        if(threads[i] != NULL){
            pthread_join(*threads[i], NULL);
        }
    }

    pthread_mutex_destroy( &mutex );
    pthread_cond_destroy ( &recursos_livres );

}















// No início da thread, ela deve receber como parâmetros seu tid, os tempos livre e crítico
//    e os recursos que ela necessita para completar seu trabalho
// Em segunda, ela deve executar as operações:
//   spend_time(tid,NULL,tlivre);
//   trava_recursos(recursos);     // a forma de representar os recursos é uma decisão do desenvolvedor
//   spend_time(tid,"C",tcritico);
//   libera_recursos();            // note que cada thread deve ser ter sua lista de recursos registrada em algum lugar
//   pthread_exit(); 