#include "feria.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRIBA 'W'
#define ABAJO 'S'
#define IZQUIERDA 'A'
#define DERECHA 'D'
#define CAMUFLARSE 'Q'
#define arriba 'w'
#define abajo 's'
#define izquierda 'a'
#define derecha 'd'
#define camuflarse 'q'
#define SIGUE_JUGANDO 0

//Devuelve verdadero si el caracter recibido es valido. Caso contrario devuelve falso.
bool es_accion_valida(char accion)
{
	if (accion == ARRIBA || accion == ABAJO || accion == DERECHA ||
	    accion == IZQUIERDA || accion == CAMUFLARSE || accion == arriba ||
	    accion == abajo || accion == derecha || accion == izquierda ||
	    accion == camuflarse) {
		return true;
	} else {
		return false;
	}
}

int main()
{
	srand((unsigned int)time(NULL));
	juego_t juego;
	char accion;
	inicializar_juego(&juego);
	imprimir_terreno(juego);
	while (estado_juego(juego) == SIGUE_JUGANDO) {
		printf("\nIngrese un movimiento: ");
		scanf("%c", &accion);
		while (!es_accion_valida(accion)) {
			printf("\nIngrese un movimiento  valido!");
			printf("\nIngrese un movimiento: ");
			scanf("%c", &accion);
		}
		system("clear");
		realizar_jugada(&juego, accion);
		imprimir_terreno(juego);
	}
	return 0;
}