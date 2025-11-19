#include "barrera.h"
#include <stdio.h>

int barrera_init(barrera_t *b, int N){
    if(N<=0){
        fprintf(stderr, "Error: N debe ser mayor que 0\n");
        return -1;
    }
    //incializo los campos del struct
    b->N=N;
    b->count=0;
    b->etapa=0;
    
    int err;

    err=pthread_mutex_init(&b->mutex, NULL);
    if(err!=0){
        return err;;
    }
    err=pthread_cond_init(&b->cond, NULL);
    if(err!=0){
        pthread_mutex_destroy(&b->mutex);
        return err;
    }
    return 0;
}

int barrera_destroy(barrera_t *b){
    int err1 = pthread_mutex_destroy(&b->mutex);
    int err2 = pthread_cond_destroy(&b->cond);
    
    if(err1 != 0) return err1;
    else return err2;
}

void barrera_wait(barrera_t *b){
    pthread_mutex_lock(&b->mutex);
    //etapa actual
    int mi_etapa = b->etapa;

    //esta hebra llega a la barrrera
    b->count++;

    if(b->count==b->N){
        //ultima hebra en llegar, avanza de etapa, resetea count y despierta a todas
        b->etapa++;
        b->count=0;
        pthread_cond_broadcast(&b->cond);
    }
    else{
        //no es la ultima hebra, espera a que lleguen las demas
        while(mi_etapa == b->etapa){
            pthread_cond_wait(&b->cond, &b->mutex);
        }
    }
    pthread_mutex_unlock(&b->mutex);
}