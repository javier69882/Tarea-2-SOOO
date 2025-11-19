#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "barrera.h"

// Struct para pasar argumentos a cada hebra
typedef struct {
    int id;            
    int etapas;     
    barrera_t *barrera; // puntero a la barrera compartida
} hebra_arg_t;

// Trabajo simulado antes de cada barrera
void trabajo_simulado(int id, int etapa, unsigned int *seed) {
    *seed = 1103515245u * (*seed + (unsigned int)id + (unsigned int)(etapa * 17u)) + 12345u;   // seed pseudoaleatoria
    useconds_t espera = (useconds_t)((*seed % 100u) + 1u) * 1000u; // entre 1ms y 100ms de espera
    usleep(espera);
}


// Función que ejecuta cada hebra
void *func_hebra(void *arg) {
    hebra_arg_t *info = (hebra_arg_t *) arg;
    int id = info->id;
    int E  = info->etapas;
    barrera_t *b = info->barrera;

    unsigned int seed = (unsigned int) time(NULL) ^ (id * 1234567u); // seed distinta por hebra

    for (int e = 0; e < E; e++) {
        
        trabajo_simulado(id, e, &seed);// simula trabajo antes de la barrera
        printf("Hebra %d: antes de barrera en etapa %d\n", id, e); // mensaje antes de barrera
        fflush(stdout);
        barrera_wait(b);  // espera en la barrera
        printf("Hebra %d: despues de barrera en etapa %d\n", id, e);  // mensaje después de barrera
        fflush(stdout);
    }
    return NULL;
}


int main(int argc, char *argv[]) {
    int N = 5; // número de hebras por defecto
    int E = 4; // número de etapas por defecto

    // Procesar argumentos de línea de comandos
    if (argc >= 2) {
        N = atoi(argv[1]);
    }
    if (argc >= 3) {
        E = atoi(argv[2]);
    }
     // Validar argumentos
    if (N <= 0 || E <= 0) {
        fprintf(stderr, "Uso: %s [N_hebras (>0)] [E_etapas (>0)]\n", argv[0]);
        return 1;
    }
    printf("Iniciando con N = %d hebras, E = %d etapas\n", N, E);

    barrera_t barrera;
    int err = barrera_init(&barrera, N);  // inicializar la barrera
    if (err != 0) {
        fprintf(stderr, "Error al inicializar la barrera (codigo %d)\n", err);
        return 1;
    }

    // Reservar memoria para hebras y argumentos
    pthread_t *hilos = malloc(sizeof(pthread_t) * N);
    hebra_arg_t *args = malloc(sizeof(hebra_arg_t) * N);

    // Verificar asignación de memoria
    if (hilos == NULL || args == NULL) {    
        fprintf(stderr, "Error al reservar memoria para hebras\n");
        barrera_destroy(&barrera);
        free(hilos);
        free(args);
        return 1;
    }

    // Crear hebras
    for (int i = 0; i < N; i++) {
        args[i].id = i;
        args[i].etapas = E;
        args[i].barrera = &barrera;
        err = pthread_create(&hilos[i], NULL, func_hebra, &args[i]);
        if (err != 0) {
            fprintf(stderr, "Error al crear hebra %d (codigo %d)\n", i, err);
            N = i;
            break;
        }
    }

    // Esperar a todas las hebras creadas
    for (int i = 0; i < N; i++) {
        pthread_join(hilos[i], NULL);
    }

    // Limpiar
    barrera_destroy(&barrera);
    free(hilos);
    free(args);

    return 0;
}