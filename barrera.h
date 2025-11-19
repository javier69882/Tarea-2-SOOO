
#ifndef BARRERA_H
#define BARRERA_H
#include <pthread.h>

// barrera que se puede reutiliza para N hebras
//Implemntacion usando mutex y variables de condicion, implementado como monitor
typedef struct{
    int count; //hebras que han llegado a la barrera acutal
    int N; //numero de hebras que deben llegar a la barrera
    int etapa; //etapa actual de la barrera
    pthread_mutex_t mutex; //para la exclusion mutua
    pthread_cond_t cond; //para la variable de condicion
} barrera_t;


//inicializa la barrera para sincronizar N hebras.
//devuelve 0 si todo va bien.
int barrera_init(barrera_t *b, int N);


//destruye la barrera
int barrera_destroy(barrera_t *b);


//bloquea a la hebra hazta que lleguen N hebras a la barrera
void barrera_wait(barrera_t *b);

#endif // BARRERA_H