#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "marcos.h"

#define decimal 10
#define hexadecimal 16
#define binario 2

int main(int argc, char *argv[]) {
    if(argc<4){
        fprintf(stderr, "Uso: %s Nmarcos tamañoMarco [--verbose] traza.txt\n", argv[0]);
        return 1;
    }

    int verbose = 0;
    int Nmarcos;
    uint64_t tamaño_pagina;
    const char *nombre_traza = NULL;

    Nmarcos = atoi(argv[1]);
    if(Nmarcos <= 0){
        fprintf(stderr, "Error: Nmarcos debe ser un entero positivo.\n");
        return 1;
    }

    errno = 0;
    tamaño_pagina = strtoull(argv[2],NULL,decimal);
    if(errno != 0 || tamaño_pagina == 0){
        fprintf(stderr, "Error: tamañoMarco incorrecto\n");
        return 1;
    }

    if (!potencia_de_dos(tamaño_pagina)) {
        fprintf(stderr, "Error: tamañomarco debe ser potencia de 2\n");
        return 1;
    }

    if (argc == 4) {
        /* ./sim Nmarcos tamañomarco traza.txt */
        nombre_traza = argv[3];
    } else {
        /* ./sim Nmarcos tamañomarco --verbose traza.txt */
        if (strcmp(argv[3], "--verbose") == 0) {
            verbose = 1;
            nombre_traza = argv[4];
        } else {
            fprintf(stderr, "Opcion no reconocida: %s\n", argv[3]);
            return EXIT_FAILURE;
        }
    }

    unsigned int b = 0;
    uint64_t tmp = tamaño_pagina;
    while (tmp > 1) {
        tmp >>= 1;
        b++;
    }
    uint64_t MASK = tamaño_pagina - 1;

    marco *marcos = calloc(Nmarcos, sizeof(marco));
    if(marcos == NULL){
        fprintf(stderr, "Error al reservar memoria para los marcos: %s\n", strerror(errno));
        free(marcos);
        return 1;
    }

    FILE *f = fopen(nombre_traza, "r");
    if(!f){
        fprintf(stderr, "Error al abrir el archivo de traza '%s': %s\n", nombre_traza, strerror(errno));
        free(marcos);
        return 1;
    }

    uint64_t total_referencias = 0;
    uint64_t fallos_pagina = 0;
    int marcos_usados = 0;
    int reloj = 0;
    
    char linea[256];
    
    while(fgets(linea, sizeof(linea), f)){
        linea[strcspn(linea, "\r")] = 0;
        char *p = linea;
        while(*p == ' ' || *p == '\t') 
            p++;
        if(*p == '\0' || *p == '\n') 
            continue;
        
        errno = 0;
        uint64_t direccion_virtual;
        
        if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
            direccion_virtual = strtoull(p, NULL, hexadecimal);
        } else {
            direccion_virtual = strtoull(p, NULL, decimal);
        }
        if (errno != 0) {
            fprintf(stderr, "Línea inválida: %s", linea);
            continue;
        }

        total_referencias++;

        uint64_t offset = direccion_virtual & MASK;
        uint64_t pagina_virtual = direccion_virtual >> b;

        int hit = 0;
        int marco = -1;
        
        for (int i = 0; i < Nmarcos; i++) {
            if (marcos[i].valido && marcos[i].pagina_virtual == pagina_virtual) {
                hit = 1;
                marco = i;
                break;
            }
        }

        if (hit) {
            marcos[marco].referencia = 1;
        } else {
            fallos_pagina++;

            if (marcos_usados < Nmarcos) {
                marco = marcos_usados;
                marcos_usados++;
            } else {
                //algoritmo de reloj
                while (1) {
                    if (!marcos[reloj].valido || marcos[reloj].referencia == 0) {
                        marco = reloj;
                        reloj = (reloj + 1) % Nmarcos;
                        break;
                    } else {
                        marcos[reloj].referencia = 0;
                        reloj = (reloj + 1) % Nmarcos;
                    }
                }
            }

            marcos[marco].valido = 1;
            marcos[marco].pagina_virtual  = pagina_virtual;
            marcos[marco].referencia = 1;
        }

        uint64_t direccion_fisica = (((uint64_t)marco) << b) | offset;

        if (verbose) {
            printf("Direccion virtual: 0x%llX\n Pagina virtual: %llu\n offset: 0x%llX \n %s\n"
                   " Marco utilizado: %d\n Direccion fisica: 0x%llX\n\n",
                   (unsigned long long)direccion_virtual,
                   (unsigned long long)pagina_virtual,
                   (unsigned long long)offset,
                   hit ? "HIT" : "FALLO",
                   marco,
                   (unsigned long long)direccion_fisica);
        }
    }
    fclose(f);

    double tasa = 0.0;
    if (total_referencias > 0) {
        tasa = (double)fallos_pagina / (double)total_referencias;
    }

    printf("\nTotales:\n");
    printf("  Referencias: %llu\n",
           (unsigned long long)total_referencias);
    printf("  Fallos de pagina: %llu\n",
           (unsigned long long)fallos_pagina);
    printf("  Tasa de fallos : %.6f\n", tasa);

    free(marcos);
    return 0;
}

