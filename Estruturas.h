#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#define L 21
#define C 61
#define nInim 5
#define nObj 0

#define ESC 27

#define SERVER_FIFO "/tmp/s_fifo"
#define CLIENT_FIFO "/tmp/cliente_%d_fifo"


WINDOW *win_menu;
WINDOW *win_game;

typedef struct labirinto Labirinto;
typedef struct jogador Jogador;
typedef struct inimigo Inimigo;
typedef struct objeto Objeto;
typedef struct partida Partida;
typedef struct posicao Posicao;
typedef struct pedido Pedido;

struct posicao{
	int x, y;
};

struct objeto{
	Posicao pos;
	char tipo;
	int contObj;
};

struct pedido{
	char comando[20];
};

struct jogador{
	Posicao pos;
	char caracter;
	char username[20];
	char password[20];
  	int PID;
	int pontos;
	int vidas;
	int login;
	Objeto obj[2];
	Pedido p;
}jogadores;

struct inimigo{
	Posicao pos;
	int contInim;
	char id;
};


struct partida{
	Jogador j[20];
	char mat[L][C]; // LABIRINTO!
	int tempoJogo;
	int n_ativos;
	int fim;
};

 

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* ESTRUTURAS_H */
