#include "feria.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MAX_BOMBAS 20
#define MAX_HERRAMIENTAS 100
#define MAX_FAMILIARES 10
#define MAX_FILAS 20
#define MAX_COLUMNAS 20
#define ARRIBA 'W'
#define ABAJO 'S'
#define IZQUIERDA 'A'
#define DERECHA 'D'
#define CAMUFLARSE 'Q'
#define TOPE_DE_BOMBAS 10
#define TOPE_DE_HERRAMIENTAS 8
#define TOPE_DE_FAMILIARES 3
const char PERRY_NORMAL = 'p';
const char PERRY_CAMUFLADO = 'P';
const char BOMBAS = 'B';
const char HERRAM_SOMBRERO = 'S';
const char HERRAM_GOLOSINA = 'G';
const char PHINEAS = 'H';
const char FERB = 'F';
const char CANDACE = 'C';
const char ROBOT = 'R';
const int DISTANCIA_PELIGRO_ROBOT = 2;
const int DISTANCIA_PELIGRO_FAMILIAR = 1;
const int POS_PHINEAS = 0;
const int POS_FERB = 1;
const int POS_CANDANCE = 2;
const int MAX_VIDAS_PERRY = 3;
const int ENERGIA_PERRY = 100;
const int PERDISTE = -1;
const int SIGUE_JUGANDO = 0;
const int GANASTE = 1;
const int TOPE_INICIAL = 0;
const int MINIMO_TIMER = 50;
const int MAXIMO_TIMER = 251;
const int MOVIMIENTOS_INICIALES = 0;
const int VIDA_EXTRA = 1;
const int ENERGIA_EXTRA = 20;
const int ENERGIA_PARA_BOMBA = 10;
const int CANTIDAD_INICIAL_ROBOTS = 0;
const int ENERGIA_PARA_ROBOT = 5;
const int MULTIPLO_DE_DIEZ = 10;

//Post:Genera una coordenada_t aleatoria dentro de los rangos de una matriz de dimensiones 20x20.
coordenada_t crear_posiciones_aleatorias()
{
	coordenada_t coordenada;
	coordenada.fil = rand() % MAX_FILAS;
	coordenada.col = rand() % MAX_COLUMNAS;
	return coordenada;
}

//Pre: La coordenada recibida debe ser válida. Juego debe apuntar a una estructura valida no nula.
//Post:Verifica si la coordenada recibida ya existe.
bool coordenada_ocupada(coordenada_t coordenada, juego_t *juego)
{
	bool ocupada = false;
	if (coordenada.fil == juego->perry.posicion.fil &&
	    coordenada.col == juego->perry.posicion.col) {
		ocupada = true;
	}
	for (int i = 0; i < juego->tope_bombas; i++) {
		if (coordenada.fil == juego->bombas[i].posicion.fil &&
		    coordenada.col == juego->bombas[i].posicion.col) {
			ocupada = true;
		}
	}
	for (int i = 0; i < (juego->cantidad_robots - 1); i++) {
		if (coordenada.fil == juego->robots[i].fil &&
		    coordenada.col == juego->robots[i].col) {
			ocupada = true;
		}
	}
	for (int i = 0; i < juego->tope_herramientas; i++) {
		if (coordenada.fil == juego->herramientas[i].posicion.fil &&
		    coordenada.col == juego->herramientas[i].posicion.col) {
			ocupada = true;
		}
	}
	for (int i = 0; i < juego->tope_familiares; i++) {
		if (coordenada.fil == juego->familiares[i].posicion.fil &&
		    coordenada.col == juego->familiares[i].posicion.col) {
			ocupada = true;
		}
	}
	return ocupada;
}

//Pre: Las coordenadas que recibe deben estar cargadas de manera válida dentro del terreno.
//Post: Devuelve la distancia entre dos coordenadas del juego.
int calcular_distancia(coordenada_t elemento, coordenada_t perry)
{
	int distancia =
		abs(elemento.fil - perry.fil) + abs(elemento.col - perry.col);
	return distancia;
}

//Pre: Juego debe apuntar a una estructura juego_t valida no nula.
//Post: Inicializa vida, energia, posicion aleatoria y el estado del camuflaje inicial de Perry.
void inicializar_perry(juego_t *juego)
{
	personaje_t personaje_perry = juego->perry;
	personaje_perry.vida = MAX_VIDAS_PERRY;
	personaje_perry.energia = ENERGIA_PERRY;
	personaje_perry.camuflado = false;
	personaje_perry.posicion = crear_posiciones_aleatorias();
	juego->perry = personaje_perry;
}

////Pre: Juego debe apuntar a una estructura juego_t valida no nula.
//Post: Para cada bomba del arreglo, inicializa posicion aleatoria válida, timer aleatorio y estado inicial.
void inicializar_bombas(juego_t *juego)
{
	int bombas_con_coord_unica = 0;
	while (bombas_con_coord_unica < TOPE_DE_BOMBAS) {
		coordenada_t coord_bomba = crear_posiciones_aleatorias();
		if (!coordenada_ocupada(coord_bomba, juego)) {
			juego->bombas[juego->tope_bombas].posicion =
				coord_bomba;
			juego->bombas[juego->tope_bombas].timer =
				(rand() % MAXIMO_TIMER + MINIMO_TIMER);
			juego->bombas[juego->tope_bombas].desactivada = false;
			juego->tope_bombas++;
			bombas_con_coord_unica++;
		}
	}
}

//Pre:Juego debe apuntar a una estructura juego_t valida no nula.
//Post: Inicializa el campo robots. Reserva memoria para los robots de manera dinámica.
void inicializar_robots(juego_t *juego)
{
	juego->robots =
		malloc(sizeof(coordenada_t) * (size_t)juego->cantidad_robots);
}

//Pre: Juego debe apuntar a una estructura juego_t valida no nula.
/*Post: Genera un robot en el juego a partir de 10 movimientos del personaje. 
  Utiliza realloc() para redimensionar la memoria que ocupa el robot.
  Para cada robot generado, se crea una posicion aleatoria válida.*/
void agregar_robot(juego_t *juego)
{
	int robot_con_coord_unica = 0;
	if ((juego->movimientos) % MULTIPLO_DE_DIEZ == 0 &&
	    juego->movimientos != 0) {
		juego->cantidad_robots++;
		juego->robots = realloc(juego->robots,
					sizeof(coordenada_t) *
						(size_t)juego->cantidad_robots);
		if (juego->robots != NULL) {
			while (robot_con_coord_unica < juego->cantidad_robots) {
				coordenada_t coord_de_robot =
					crear_posiciones_aleatorias();
				if (!coordenada_ocupada(coord_de_robot,
							juego) &&
				    juego->cantidad_robots != 0) {
					juego->robots[juego->cantidad_robots -
						      1] = coord_de_robot;
					robot_con_coord_unica++;
				}
			}
		}
	}
}

//Pre: Juego debe apuntar a una estructura juego_t válida no nula. la posición recibida debe ser válida.
//Post: Elimina el robot del arreglo. Actualiza el vector con la cantidad de robots en el juego.
void eliminar_robot(juego_t *juego, int posicion_robot)
{
	for (int i = posicion_robot + 1; i < juego->cantidad_robots; i++) {
		juego->robots[i - 1] = juego->robots[i];
	}
	juego->cantidad_robots--;
}

//Pre: Juego debe apuntar a una estructura juego_t válida no nula.
//Post: Maneja las funcionalidades del robot. Segun el estado del personaje Perry, destruye el robot y lo elimina del juego.
void comportamiento_robots(juego_t *juego)
{
	for (int i = 0; i < juego->cantidad_robots; i++) {
		if (calcular_distancia(juego->robots[i],
				       juego->perry.posicion) <=
		    DISTANCIA_PELIGRO_ROBOT) {
			if (juego->perry.camuflado) {
				juego->perry.vida--;
			} else if (!juego->perry.camuflado &&
				   juego->perry.energia >= ENERGIA_PARA_ROBOT) {
				juego->perry.energia -= ENERGIA_PARA_ROBOT;
				eliminar_robot(juego, i);
			} else if (!juego->perry.camuflado &&
				   juego->perry.energia < ENERGIA_PARA_ROBOT) {
				juego->perry.vida--;
				eliminar_robot(juego, i);
			}
		}
	}
}

//Pre: Juego debe apuntar a una estructura juego_t valida no nula.
/*Post: Para cada herramienta dentro del vector, inicializa posicion aleatoria válida dentro del terreno.
  Asigna tipo de herramienta según corresponda.*/
void inicializar_herramientas(juego_t *juego)
{
	int herram_con_coord_unica = 0;
	while (herram_con_coord_unica < TOPE_DE_HERRAMIENTAS) {
		coordenada_t coord_de_herram = crear_posiciones_aleatorias();
		if (!coordenada_ocupada(coord_de_herram, juego)) {
			juego->herramientas[juego->tope_herramientas].posicion =
				coord_de_herram;
			juego->tope_herramientas++;
			herram_con_coord_unica++;
		}
	}
	for (int i = 0; i < juego->tope_herramientas; i++) {
		if (i < 3) {
			juego->herramientas[i].tipo = HERRAM_SOMBRERO;
		} else {
			juego->herramientas[i].tipo = HERRAM_GOLOSINA;
		}
	}
}

//Pre: Juego debe apuntar a una estructura juego_t valida no nula.
/*Post: Para cada familiar dentro del arreglo, inicializa posicion aleatoria válida dentro del terreno.
  Asigna inicial de nombre en órden.
  Asigna sentido de movimiento según cooresponde.*/
void inicializar_familiares(juego_t *juego)
{
	int fam_con_coord_unica = 0;
	while (fam_con_coord_unica < TOPE_DE_FAMILIARES) {
		coordenada_t coord_de_familiar = crear_posiciones_aleatorias();
		if (!coordenada_ocupada(coord_de_familiar, juego)) {
			juego->familiares[juego->tope_familiares].posicion =
				coord_de_familiar;
			juego->tope_familiares++;
			fam_con_coord_unica++;
		}
	}
	for (int i = 0; i < juego->tope_familiares; i++) {
		if (i == POS_PHINEAS) {
			juego->familiares[i].inicial_nombre = PHINEAS;
			juego->familiares[i].sentido = DERECHA;
		} else if (i == POS_FERB) {
			juego->familiares[i].inicial_nombre = FERB;
			juego->familiares[i].sentido = IZQUIERDA;
		} else {
			juego->familiares[i].inicial_nombre = CANDACE;
			juego->familiares[i].sentido = ARRIBA;
		}
	}
}

//Pre: Juego debe apuntar a una estructura juego_t valida no nula.
//Post: Inicializa todos los campos de juego_t.
void inicializar_juego(juego_t *juego)
{
	juego->cantidad_robots = CANTIDAD_INICIAL_ROBOTS;
	juego->movimientos = MOVIMIENTOS_INICIALES;
	juego->tope_bombas = TOPE_INICIAL;
	juego->tope_herramientas = TOPE_INICIAL;
	juego->tope_familiares = TOPE_INICIAL;
	inicializar_perry(juego);
	inicializar_bombas(juego);
	inicializar_robots(juego);
	inicializar_herramientas(juego);
	inicializar_familiares(juego);
}

//Post: Inicializa vacía la matriz recibida.
void inicializar_terreno_vacio(char terreno[MAX_FILAS][MAX_COLUMNAS])
{
	for (int i = 0; i < MAX_FILAS; i++) {
		for (int j = 0; j < MAX_COLUMNAS; j++) {
			terreno[i][j] = ' ';
		}
	}
}

//Pre: El campo perry.posicion debe tener cargada una coordenada válida en las dimensiones del terreno.
/*Post: Carga el campo "posicion" de personaje_t dentro de la matriz recibida. 
  Dependiendo del valor del campo "camuflado" de personaje_t, carga el char 'P' o 'p'*/
void cargar_perry_terreno(personaje_t perry,
			  char terreno[MAX_FILAS][MAX_COLUMNAS])
{
	int fila_perry = perry.posicion.fil;
	int col_perry = perry.posicion.col;
	terreno[fila_perry][col_perry] = PERRY_NORMAL;
	if (perry.camuflado) {
		terreno[fila_perry][col_perry] = PERRY_CAMUFLADO;
	}
}

//Pre: Para cada bomba del arreglo, el campo posicion debe tener cargada una coordenada válida en las dimensiones del terreno.
/*Post: Carga el campo "posición" de cada bomba_t dentro de la matriz recibida.
  Dependiendo del valor del campo "desactivada", carga char 'B' o ' '*/
void cargar_bombas_terreno(bomba_t bombas[MAX_BOMBAS],
			   char terreno[MAX_FILAS][MAX_COLUMNAS])
{
	for (int i = 0; i < TOPE_DE_BOMBAS; i++) {
		int fila_bomba = bombas[i].posicion.fil;
		int col_bomba = bombas[i].posicion.col;
		if (!bombas[i].desactivada) {
			terreno[fila_bomba][col_bomba] = BOMBAS;
		} else {
			terreno[fila_bomba][col_bomba] = ' ';
		}
	}
}

//Pre: Para cada robot en el arreglo, se debe tener cargada una coordenada válida en las dimensiones del terreno.
//Post: Carga los robots dentro de la matriz recibida.
void cargar_robot_terreno(juego_t juego, char terreno[MAX_FILAS][MAX_COLUMNAS])
{
	if (juego.cantidad_robots > 0) {
		for (int i = 0; i < juego.cantidad_robots; i++) {
			int fila_robot = juego.robots[i].fil;
			int col_robot = juego.robots[i].col;
			terreno[fila_robot][col_robot] = ROBOT;
		}
	}
}

//Pre: Para cada herramienta en el arreglo, el campo posicion debe tener cargada una coordenada válida en las dimensiones del terreno.
/*Post: Carga el campo posicion de herramienta_t dentro de la matriz recibida.
  Dependiendo el tipo, asignará char 'S' o 'G'*/
void cargar_herramientas_terreno(juego_t juego,
				 char terreno[MAX_FILAS][MAX_COLUMNAS])
{
	for (int i = 0; i < juego.tope_herramientas; i++) {
		int fila_herram = juego.herramientas[i].posicion.fil;
		int col_herram = juego.herramientas[i].posicion.col;
		char tipo_herram = juego.herramientas[i].tipo;
		terreno[fila_herram][col_herram] = tipo_herram;
	}
}

//Pre: Para cada familiar en el arreglo, el campo posicion debe tener cargada una coordenada válida en las dimensiones del terreno.
/*Post: Carga el campo posicion de familiar_t dentro de la matriz recibida.
  Dependiendo del campo 'inicial_nombre' asignará char 'H', 'F' o 'C'*/
void cargar_familia_terreno(familiar_t familiares[MAX_FAMILIARES],
			    char terreno[MAX_FILAS][MAX_COLUMNAS])
{
	for (int i = 0; i < TOPE_DE_FAMILIARES; i++) {
		int fila_familiar = familiares[i].posicion.fil;
		int col_familiar = familiares[i].posicion.col;
		char inicial_nombre = familiares[i].inicial_nombre;
		terreno[fila_familiar][col_familiar] = inicial_nombre;
	}
}

//Explica el juego al usuario.
void modo_juego()
{
	printf("<<<¡Salvemos a Danville del malvado Dr. Doofenshmirtz!>>>");
	printf("\nModo de juego: Camuflate cuando(H),(F) o (C) esten a un movimiento de distancia.");
	printf("\nMuevete por la ciudad: hacia arriba (W), abajo (S), derecha (D), izquierda (A). Para camuflarte pulsa (Q).");
	printf("\nPara desactivar las bombas(B) Perry no debe estar camuflado!!");
}

//Pre: juego deberá ser una copia válida de la estructura juego_t.
//Post: Muestra vidas y energía restantes al jugador. Indica si Perry está o no camuflado.
void info_al_usuario(juego_t juego)
{
	printf("\nTenes: %d vidas ♥.", juego.perry.vida);
	printf("\nTe quedan : %d puntos de energía.", juego.perry.energia);
	if (juego.perry.camuflado) {
		printf("\nEstas camuflado!");
	} else {
		printf("\nNo estas camuflado, ojo!");
	}
}

//Pre: juego deberá ser una copia válida de la estructura juego_t. El terreno deberá estar correctamente inicializado.
void llamar_funciones_de_terreno(juego_t juego,
				 char terreno[MAX_FILAS][MAX_COLUMNAS])
{
	inicializar_terreno_vacio(terreno);
	cargar_bombas_terreno(juego.bombas, terreno);
	cargar_herramientas_terreno(juego, terreno);
	cargar_robot_terreno(juego, terreno);
	cargar_familia_terreno(juego.familiares, terreno);
	cargar_perry_terreno(juego.perry, terreno);
}

//Pre: juego deberá ser una copia válida de la estructura juego_t.
//Post: Carga todos los elementos del juego en el terreno.
void imprimir_terreno(juego_t juego)
{
	char terreno[MAX_FILAS][MAX_COLUMNAS];
	modo_juego();
	llamar_funciones_de_terreno(juego, terreno);
	printf("\n");
	printf("\n♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦");
	printf("\n");
	for (int i = 0; i < MAX_FILAS; i++) {
		for (int j = 0; j < MAX_COLUMNAS; j++) {
			if (terreno[i][j] == PERRY_NORMAL ||
			    terreno[i][j] == PERRY_CAMUFLADO) {
				printf("\033[0;34m %c\033[0m ", terreno[i][j]);
			} else if (terreno[i][j] == PHINEAS) {
				printf("\033[0;33m %c\033[0m ", terreno[i][j]);
			} else if (terreno[i][j] == FERB) {
				printf("\033[0;32m %c\033[0m ", terreno[i][j]);
			} else if (terreno[i][j] == CANDACE) {
				printf("\033[0;35m %c\033[0m ", terreno[i][j]);
			} else if (terreno[i][j] == ROBOT) {
				printf("\033[0;31m %c\033[0m ", terreno[i][j]);
			} else {
				printf("[%c]", terreno[i][j]);
			}
		}
		printf("\n");
	}
	printf("♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦♦");
	info_al_usuario(juego);
}

//Pre: La coordenada recibida debe ser válida.
//Post: Verifica si el personaje puede realizar un movimiento en el terreno, sin salirse de los límites del mismo.
bool perry_puede_moverse(coordenada_t coordenada_perry)
{
	if (coordenada_perry.fil == 0 ||
	    coordenada_perry.fil == (MAX_FILAS - 1) ||
	    coordenada_perry.col == 0 ||
	    coordenada_perry.col == (MAX_COLUMNAS - 1)) {
		return false;
	} else {
		return true;
	}
}

//Post: Devuelve el caracter recibido convertido a letra mayuscula.
char convertir_a_mayuscula(char caracter)
{
	return ((char)toupper(caracter));
}

//Pre: Recibe vector inicializado correctamente. El tope debe estar actualizado e inicializado.
//Post: Elimina herramienta especifica del arreglo, decrementando el tope.
void eliminar_herramienta_usada(herramienta_t herramientas[MAX_HERRAMIENTAS],
				int *tope_herramientas, int posicion_a_eliminar)
{
	if (posicion_a_eliminar >= *tope_herramientas) {
		return;
	}
	for (int i = posicion_a_eliminar + 1; i < *tope_herramientas; i++) {
		herramientas[i - 1] = herramientas[i];
	}
	(*tope_herramientas)--;
}

//Pre: Juego debe apuntar a una estructura juego_t valida no nula.
//Post: Perry gana una vida o energia al usar una herramienta. La herramienta usada se elimina del juego.
void usar_herramientas(juego_t *juego)
{
	for (int i = 0; i < juego->tope_herramientas; i++) {
		if (juego->herramientas[i].posicion.fil ==
			    juego->perry.posicion.fil &&
		    juego->herramientas[i].posicion.col ==
			    juego->perry.posicion.col) {
			if (juego->herramientas[i].tipo == HERRAM_SOMBRERO) {
				juego->perry.vida += VIDA_EXTRA;
			} else if (juego->herramientas[i].tipo ==
				   HERRAM_GOLOSINA) {
				juego->perry.energia += ENERGIA_EXTRA;
			}
			eliminar_herramienta_usada(juego->herramientas,
						   &juego->tope_herramientas,
						   i);
		}
	}
}

//Pre: Juego debe apuntar a una estructura juego_t valida no nula.
//Post: Bajo las condiciones necesarias, al pasar por una bomba, Perry la desactiva y pierde energia.
void desactivar_bombas(juego_t *juego)
{
	for (int i = 0; i < juego->tope_bombas; i++) {
		if (juego->bombas[i].posicion.fil ==
			    juego->perry.posicion.fil &&
		    juego->bombas[i].posicion.col ==
			    juego->perry.posicion.col) {
			if (!juego->perry.camuflado &&
			    juego->perry.energia >= ENERGIA_PARA_BOMBA) {
				juego->bombas[i].desactivada = true;
				juego->perry.energia -= ENERGIA_PARA_BOMBA;
			}
		}
	}
}

//Pre: Juego debe apuntar a una estructura juego_t válida. Las bombas deben tener el timer correctamente inicializado.
/*Post: Se descuenta 1 del timer por cada movimiento de Perry. 
  Si el timer llega a cero, la bomba explota, resta una vida al personaje y se desactiva*/
void timer_bombas(juego_t *juego)
{
	for (int i = 0; i < juego->tope_bombas; i++) {
		if (!juego->bombas[i].desactivada) {
			juego->bombas[i].timer--;
		}
		if (juego->bombas[i].timer == 0 &&
		    !juego->bombas[i].desactivada) {
			juego->perry.vida--;
			juego->bombas[i].desactivada = true;
		}
	}
}

//Pre: Juego apunta a una estructura juego_t válida. Phineas debe estar inicializado correctamente.
//Post: Maneja el sentido del movimiento de Phineas dentro del terreno, comenzando con sentido hacia la derecha.
void movimiento_phineas(juego_t *juego)
{
	if (juego->familiares[POS_PHINEAS].sentido == DERECHA) {
		if (juego->familiares[POS_PHINEAS].posicion.col <
		    (MAX_FILAS - 1)) {
			juego->familiares[POS_PHINEAS].posicion.col++;
		} else {
			juego->familiares[POS_PHINEAS].sentido = IZQUIERDA;
			juego->familiares[POS_PHINEAS].posicion.col--;
		}
	} else {
		if (juego->familiares[POS_PHINEAS].posicion.col > 0) {
			juego->familiares[POS_PHINEAS].posicion.col--;
		} else {
			juego->familiares[POS_PHINEAS].sentido = DERECHA;
			juego->familiares[POS_PHINEAS].posicion.col++;
		}
	}
}

//Pre: Juego apunta a una estructura juego_t válida. Ferb debe estar inicializado correctamente.
//Post: Maneja el sentido del movimiento de Ferb dentro del terreno, comenzando con sentido hacia la izquierda.
void movimiento_ferb(juego_t *juego)
{
	if (juego->familiares[POS_FERB].sentido == IZQUIERDA) {
		if (juego->familiares[POS_FERB].posicion.col > 0) {
			juego->familiares[POS_FERB].posicion.col--;
		} else {
			juego->familiares[POS_FERB].sentido = DERECHA;
			juego->familiares[POS_FERB].posicion.col++;
		}
	} else {
		if (juego->familiares[POS_FERB].posicion.col <
		    (MAX_COLUMNAS - 1)) {
			juego->familiares[POS_FERB].posicion.col++;
		} else {
			juego->familiares[POS_FERB].sentido = IZQUIERDA;
			juego->familiares[POS_FERB].posicion.col--;
		}
	}
}

//Pre: Juego apunta a una estructura juego_t válida. Candance debe estar inicializada correctamente.
//Post: Maneja el sentido del movimiento de Candance dentro del terreno, comenzando con sentido hacia la
void movimiento_candance(juego_t *juego)
{
	if (juego->familiares[POS_CANDANCE].sentido == ARRIBA) {
		if (juego->familiares[POS_CANDANCE].posicion.fil > 0) {
			juego->familiares[POS_CANDANCE].posicion.fil--;
		} else {
			juego->familiares[POS_CANDANCE].sentido = ABAJO;
			juego->familiares[POS_CANDANCE].posicion.fil++;
		}
	} else {
		if (juego->familiares[POS_CANDANCE].posicion.fil <
		    (MAX_FILAS - 1)) {
			juego->familiares[POS_CANDANCE].posicion.fil++;
		} else {
			juego->familiares[POS_CANDANCE].sentido = ARRIBA;
			juego->familiares[POS_CANDANCE].posicion.fil--;
		}
	}
}

//Pre: juego debe apuntar a una estructura juego_t válida no nula.
/*Post: Si la distancia entre el personaje y un familiar es menor o igual a uno y no está camuflado,
  le resta una vida a Perry*/
void pierde_vida(juego_t *juego)
{
	int distancia_phineas = calcular_distancia(
		juego->familiares[POS_PHINEAS].posicion, juego->perry.posicion);
	int distancia_ferb = calcular_distancia(
		juego->familiares[POS_FERB].posicion, juego->perry.posicion);
	int distancia_candance =
		calcular_distancia(juego->familiares[POS_CANDANCE].posicion,
				   juego->perry.posicion);
	if (distancia_phineas <= DISTANCIA_PELIGRO_FAMILIAR &&
	    !juego->perry.camuflado) {
		juego->perry.vida--;
	} else if (distancia_ferb <= 1 && !juego->perry.camuflado) {
		juego->perry.vida--;
	} else if (distancia_candance <= 1 && !juego->perry.camuflado) {
		juego->perry.vida--;
	}
}

//Pre: juego debe apuntar a una estructura juego_t válida no nula.
void llamar_funciones(juego_t *juego)
{
	agregar_robot(juego);
	desactivar_bombas(juego);
	timer_bombas(juego);
	usar_herramientas(juego);
	movimiento_phineas(juego);
	movimiento_ferb(juego);
	movimiento_candance(juego);
	pierde_vida(juego);
}

//Pre:juego debe apuntar a una estructura juego_t válida no nula.La accion recibida debe ser valida.
/*Post: Luego de realizar una acción en caso de chocar o estar a la distancia de reacción con un elemento se activará la
  reacción relacionada al mismo que afectará al personaje y el estado del juego.*/
void realizar_jugada(juego_t *juego, char accion)
{
	coordenada_t mov_perry = juego->perry.posicion;
	bool perry_puede_mov = perry_puede_moverse(mov_perry);
	char accion_para_jugar = convertir_a_mayuscula(accion);
	comportamiento_robots(juego);
	switch (accion_para_jugar) {
	case CAMUFLARSE:
		juego->perry.camuflado = (!juego->perry.camuflado);
		break;
	case DERECHA:
		if (perry_puede_mov || (mov_perry.col) < (MAX_COLUMNAS - 1)) {
			mov_perry.col++;
			juego->movimientos++;
		}
		break;
	case IZQUIERDA:
		if (perry_puede_mov || (mov_perry.col) != 0) {
			mov_perry.col--;
			juego->movimientos++;
		}
		break;
	case ARRIBA:
		if (perry_puede_mov || (mov_perry.fil) != 0) {
			mov_perry.fil--;
			juego->movimientos++;
		}
		break;
	case ABAJO:
		if (perry_puede_mov || (mov_perry.fil) < (MAX_FILAS - 1)) {
			mov_perry.fil++;
			juego->movimientos++;
		}
		break;
	}
	juego->perry.posicion = mov_perry;
	llamar_funciones(juego);
}

//Pre: Se asume que 'personaje_t perry' es válido.
//Post: Evalua la cantidad de vida del personaje.
bool perry_tiene_vidas(personaje_t perry)
{
	if (perry.vida != 0) {
		return true;
	} else {
		return false;
	}
}

//Pre: El arreglo de bombas debe ser válido. El tamaño del tope no debe exceder el tamaño del arreglo.
//Post: Devuelve verdadero si todas las bombas estan desactivadas, caso contrario devuelve falso.
bool bombas_estan_desactivadas(bomba_t bombas[MAX_BOMBAS])
{
	int bombas_desactivadas = 0;
	for (int i = 0; i < TOPE_DE_BOMBAS; i++) {
		if (bombas[i].desactivada) {
			bombas_desactivadas++;
		}
	}
	return (bombas_desactivadas == TOPE_DE_BOMBAS);
}

/*
* El juego se dará por ganado cuando estén todas las bombas desactivadas.
* Si el personaje se queda sin vidas , el juego se dará por perdido.
* Devuelve:
* --> 1 si es ganado
* --> -1 si es perdido
* --> 0 si se sigue jugando
* En caso de ganado o perdido esta funcion libera la memoria reservada a lo largo del juego.
*/
int estado_juego(juego_t juego)
{
	bool tiene_vidas = perry_tiene_vidas(juego.perry);
	bool bombas_desactivas = bombas_estan_desactivadas(juego.bombas);
	if (tiene_vidas && !bombas_desactivas) {
		return SIGUE_JUGANDO;
	} else if (tiene_vidas && bombas_desactivas) {
		if (juego.robots != NULL) {
			free(juego.robots);
		}
		return GANASTE;
	} else {
		if (juego.robots != NULL) {
			free(juego.robots);
		}
		return PERDISTE;
	}
}