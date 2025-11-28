#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef MARCOS_H
#define MARCOS_H

typedef struct {
    uint64_t pagina_virtual;
    int valido;
    int referencia;
} marco;

static int potencia_de_dos(uint64_t n){
    return n && ((n & (n - 1))==0);
};
#endif