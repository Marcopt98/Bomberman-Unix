#include "Estruturas.h"
int s_fifo_fd, c_fifo_fd;
pthread_mutex_t inimigo;
pthread_mutex_t explosao;
pthread_t inimigos[nInim]; //thread para inimigos
//pthread_t explosao_p[20];
//pthread_t explosao_g[20];

Partida p;
int FIM = 0;

void trataSig(int i){
    printf("\nServidor a terminar (interrompido via teclado)\n\n"); // Tratamento para encerrar o Servidor Corretamente
    close(s_fifo_fd);
    unlink(SERVER_FIFO);
    exit(EXIT_SUCCESS);
}
//===================================== RETORNA UM VALOR ALEATORIO ENTRE MIN E MAX ==============================
int random_l_h(int min, int max){
    return min + rand() % (max-min+1);
}

/*
void *incrementa_segundos(void *dados){

	Partida *p;

	p = (Partida *)dados;

	do
	{
		sleep(1);
		p->tempoJogo++;

		if(p->)
			p->fim=1;

	}while(p->fim!=1);
}
*/

//===================================== VERIFICA SE ENTRA ==============================
int verificaLogin(char* nomefich,char user[20],char pass[20],Jogador *j,int tam){
    FILE *f;
    char u[20],p[20];
    int i;

    if((f = fopen(nomefich,"rt")) == 0){
        printf("Erro ao abrir o ficheiro.\n");
        return -1;
    }
    while (fscanf(f,"%s %s",u,p) != EOF){
        if(strcmp(u,user) == 0 && strcmp(p,pass) == 0){
          for(i=0; i < tam; i++){
              if (strcmp(user,j[i].username) == 0) {
                  printf("O Jogador %s ja esta em jogo!\n",user);
                  return -1;
              }
          }
          printf("O Jogador %s entrou com sucesso!\n",user);
          fclose(f);
          return 1;
        }
    }
    printf("Username ou password incorretos.\n");
    fclose(f);
    return -1;
}

//===================================== ESCREVE NO FICH OS USERS ==============================
void escreveFicheiro(char* nomefich, char user[20], char pass[20]){
    FILE *f;
    char checkUser[20];

    if(access(nomefich, F_OK) == -1){
        if((f = fopen(nomefich,"wt")) == NULL){
            printf("Erro a criar o ficheiro!\n");
            return;
        }
    }
    else{
        if((f = fopen(nomefich,"rt")) == 0){
            printf("Erro a abrir o ficheiro!\n");
            return;
        }
        while(fscanf(f," %s", checkUser) == 1){

            if(strcmp(user,checkUser) == 0){
                printf("O jogador ja esta registado!\n");
                fclose(f);
                return;
            }
        }
        if((f = fopen(nomefich,"at")) == NULL){
            printf("Erro a abrir o ficheiro!\n");
            return;
        }

    }

    fprintf(f, "%s %s\n", user, pass);
    printf("Jogador adicionado com sucesso!\n");

    fclose(f);
}

//===================================== MOSTRA OS JOGADORES ATIVOS ==============================
void listarUsers(Jogador *j,int tam){
    int i;
    printf("========================================\n");
    printf("              JOGADORES ON              \n");
    printf("========================================\n");
    if (tam == 0) {
        printf("Nao ha jogadores ativos.\n");
    }
    for (i=0;i<tam;i++) {
        printf("Jogador %d -> User: %s \n",i+1,j[i].username);
    }
    return;
}

//===================================== MOSTRA DETALHES DO JOGO ==============================
void mostra_info (Jogador *j, int tam){
     int i;
    printf("========================================\n");
    printf("          ESTATISTICAS DO JOGO           \n");
    printf("========================================\n");
    if (tam == 0) {
        printf("Nao ha jogadores ativos.\n");
        return;
    }
    
    for (i=0;i<tam;i++) {
        printf("PID : %d | Jogador: %s | Vidas: %d | Bombas grandes: %d | Bombas pequenas: %d \n",j[i].PID,j[i].username,j[i].vidas,j[i].obj[1].contObj,j[i].obj[0].contObj);
    }
    return;
}

//===================================== EXPULSA JOGADOR DO JOGO ==============================
Jogador* expulsaJogador(char user[20],Jogador *j,int tam,int *verifica){
    int i,k;
    union sigval val;
    *verifica = -1;

    for(i = 0; i < tam; i++){
      if(strcmp(user,j[i].username) == 0){
          *verifica = 1;
	      val.sival_int = 1;
          sigqueue(j[i].PID, SIGUSR1,val);  //Envia sinal para o cliente que foi expulso
          for (k = i; k < tam; k++){
              j[k] = j[k+1];
          }
          printf("%s saiu do jogo!\n",user);
          return j;
      }
    }
    printf("O jogador que mencionou nao existe!\n");
    return j;
}

//===================================== ALOCA MEMORIA PARA O VETOR ==============================
Jogador *criaVetor (Jogador *j,int tam){
    if(tam == 0)
	  free(j);
    if(tam == 1)
          return malloc(sizeof(Jogador) * tam);
    if(tam > 1)
          return realloc(j,sizeof(Jogador) * tam);

}

//===================================== CARREGA MAPA DO TXT ==============================
void carrega_mapa_txt(char *nomefich){
    FILE *f;
    char mat_txt[L][C];
    int i,j;
    
    if((f = fopen(nomefich,"rt")) == NULL){
        printf("Erro ao abrir o ficheiro de texto do mapa.\n");
        return;
    }
    
    for (i=0; i<L; i++){
        for(j=0; j<C; j++){
            if(!fscanf(f,"%c",&mat_txt[i][j]))
               break;
         // printf("%c",mat_txt[i][j]);
        }
        //printf("\n");
    }
    
    for (i=0; i<L; i++){
        for(j=0; j<C; j++){
          printf("%c",mat_txt[i][j]);
        }
        printf("\n");
    }
    
    
    
    fclose(f);
}

//===================================== TRATA DOS COMANDOS DO ADMIN ==============================
void tratateclado(char* nomefich, Jogador *j,int *tam){
        char comando [50],guarda[10],user[20],pass[20],nomefich_mapa[20];
        int verifica,i;
        union sigval val;

	    setbuf(stdout,NULL);

        scanf(" %[^\n]", comando);

        sscanf(comando, " %s", guarda);


        if ((strcmp(guarda,"add")) == 0){
            if(sscanf(comando, " %s %s %s", guarda, user, pass) < 2)
                printf("Insira um Username e uma Password!\n");
            else if(sscanf(comando, " %s %s %s", guarda, user, pass) < 3)
                printf("Insira uma Password!\n");
            else if (strlen(pass) < 3)
                printf("A Password tem que ter um minimo de 3 caracteres!\n");
            else if (strlen(user) > 20)
                printf("O username excede o limite maximo de 20 caracteres!\n");
            else if (strlen(pass) > 20)
                printf("A password excede o limite maximo de 20 caracteres!\n");
            else
                escreveFicheiro(nomefich, user, pass);

        }
        if ((strcmp(guarda,"users")) == 0){
            listarUsers(j,*tam);
        }
        if ((strcmp(guarda, "kick")) == 0){
            if(sscanf(comando, " %s %s", guarda, user) < 2)
                printf("Insira um utilizador a expulsar.\n");
            else{
                j = expulsaJogador (user,j,*tam,&verifica);
                if(verifica != -1){
                    *tam = *tam - 1;
                    j = criaVetor (j,*tam);
                }
            }
        }
        if ((strcmp(guarda, "game")) == 0){
            mostra_info(j,*tam);
        }
        if ((strcmp(guarda, "shutdown")) == 0){
            val.sival_int = 1;
            for(i=0; i < (*tam); i++){
                sigqueue(j[i].PID, SIGUSR2, val); //Envia Sinal para todos os clientes
            }
            FIM = 1;
            //DESTROI AS THREADS
            for(i=0; i < nInim;i++){
                pthread_join(inimigos[i],NULL);   
            }
            
            close(s_fifo_fd);
            unlink(SERVER_FIFO);
	    if((*tam) != 0)
            	criaVetor(j,0);
            printf("SERVER OFF\n");
            exit(EXIT_SUCCESS);
        }
        if ((strcmp(guarda, "map")) == 0){
            if(sscanf(comando, " %s %s", guarda, nomefich_mapa) < 2)
                printf("Insira o nome do ficheiro para o mapa.\n");
            else
                carrega_mapa_txt(nomefich_mapa);
            
        }
        if(strcmp(guarda,"help") == 0){
            printf("Lista de comandos:\n");
            printf("add user pass -> Adicionar um player\n");
            printf("users         -> Lista dos players em jogo\n");
            printf("kick          -> Expulsa um player do jogo\n");
            printf("game          -> Vai para o jogo\n");
            printf("shutdown      -> Termina o Servidor\n");
            printf("map           -> Adiciona um mapa ao jogo a partir de um ficheiro de text\n");
        }

    return;
}

//===================================== ENVIA O JOGO A TODOS OS CLIENTES ==============================

void envia_jogo(Jogador *j,int n_jogadores){
        int fd_resp,i,px,py,teste;
	
        char fifo_cliente[80];
	//Partida p;


	for(i=0; i<n_jogadores;i++){
		p.j[i] = j[i];
	}


    
    p.n_ativos = n_jogadores;
    
    for(i=0; i<n_jogadores; i++){
         sprintf(fifo_cliente,CLIENT_FIFO,p.j[i].PID);
         fd_resp = open (fifo_cliente, O_WRONLY);
        
         //printf("Enviar jogo ao PID: %d\n",j[i].PID);
         

         teste = write(fd_resp,&p,sizeof(Partida));

         
         //printf("Mapa enviado a %d \n",j[i].PID);
         //printf("Posicoes enviadas ao cliente %d | x: %d | y: %d\n",j[i].PID,j[i].pos.x,j[i].pos.y);

        close(fd_resp);            
        }
   
        
}


//===================================== THREAD DA EXPLOSAO PEQUENA ==============================
void* cria_explosao_p (void* dados){
    Jogador *j;
    int k;
    j = (Jogador *) dados;
    
    //explosão
    
   // pthread_mutex_lock(&explosao);
    // Espera 2 segundos, faz a explosão e envia aos clientes, depois espera
    // mais 2 segundos e apaga a explosao e envia aos clientes
        sleep(2);
            for(k=1;k<=2;k++){
                    //p.mat[j->obj[1].pos.x][j->obj[0].pos.y] = '@';
                if(p.mat[(j->obj[0].pos.x) - k][j->obj[0].pos.y] != '#')
                    p.mat[(j->obj[0].pos.x) - k][j->obj[0].pos.y] = '@';
                if(p.mat[(j->obj[0].pos.x) + k][j->obj[0].pos.y] != '#')
                    p.mat[(j->obj[0].pos.x) + k][j->obj[0].pos.y] = '@';
                if(p.mat[j->obj[0].pos.x][(j->obj[0].pos.y) - k] != '#')
                    p.mat[j->obj[0].pos.x][(j->obj[0].pos.y) - k] = '@';
                if(p.mat[j->obj[0].pos.x][(j->obj[0].pos.y) + k] != '#')
                    p.mat[j->obj[0].pos.x][(j->obj[0].pos.y) + k] = '@';
            }
        
            envia_jogo(j,p.n_ativos);
            sleep(2);
    
            for(k=1;k<=2;k++){
                    //p.mat[j->obj[1].pos.x][j->obj[0].pos.y] = ' ';
                if(p.mat[(j->obj[0].pos.x) - k][j->obj[0].pos.y] != '#')
                    p.mat[(j->obj[0].pos.x) - k][j->obj[0].pos.y] = ' ';
                if(p.mat[(j->obj[0].pos.x) + k][j->obj[0].pos.y] != '#')
                    p.mat[(j->obj[0].pos.x) + k][j->obj[0].pos.y] = ' ';
                if(p.mat[j->obj[0].pos.x][(j->obj[0].pos.y) - k] != '#')
                    p.mat[j->obj[0].pos.x][(j->obj[0].pos.y) - k] = ' ';
                if(p.mat[j->obj[0].pos.x][(j->obj[0].pos.y) + k] != '#')
                    p.mat[j->obj[0].pos.x][(j->obj[0].pos.y) + k] = ' ';
            }
    
    
    envia_jogo(j,p.n_ativos);
    //pthread_mutex_unlock(&explosao);
    pthread_exit(0);
}

//===================================== THREAD DA EXPLOSAO GRANDE ==============================
void* cria_explosao_g (void* dados){
    Jogador *j;
    int k;
    j = (Jogador *) dados;
    
    //explosão
    //pthread_mutex_lock(&explosao);
    // Espera 2 segundos, faz a explosão e envia aos clientes, depois espera
    // mais 2 segundos e apaga a explosao e envia aos clientes
        sleep(2);
            for(k=1;k<=4;k++){
                    //p.mat[j->obj[1].pos.x][j->obj[0].pos.y] = '@';
                if(p.mat[(j->obj[1].pos.x) - k][j->obj[1].pos.y] != '#')
                    p.mat[(j->obj[1].pos.x) - k][j->obj[1].pos.y] = '@';
                if(p.mat[(j->obj[1].pos.x) + k][j->obj[1].pos.y] != '#')
                    p.mat[(j->obj[1].pos.x) + k][j->obj[1].pos.y] = '@';
                if(p.mat[j->obj[1].pos.x][(j->obj[1].pos.y) - k] != '#')
                    p.mat[j->obj[1].pos.x][(j->obj[1].pos.y) - k] = '@';
                if(p.mat[j->obj[1].pos.x][(j->obj[1].pos.y) + k] != '#')
                    p.mat[j->obj[1].pos.x][(j->obj[1].pos.y) + k] = '@';
            }
        
            envia_jogo(j,p.n_ativos);
            sleep(2);
    
            for(k=1;k<=4;k++){
                    //p.mat[j->obj[1].pos.x][j->obj[0].pos.y] = ' ';
                if(p.mat[(j->obj[1].pos.x) - k][j->obj[1].pos.y] != '#')
                    p.mat[(j->obj[1].pos.x) - k][j->obj[1].pos.y] = ' ';
                if(p.mat[(j->obj[1].pos.x) + k][j->obj[1].pos.y] != '#')
                    p.mat[(j->obj[1].pos.x) + k][j->obj[1].pos.y] = ' ';
                if(p.mat[j->obj[1].pos.x][(j->obj[1].pos.y) - k] != '#')
                    p.mat[j->obj[1].pos.x][(j->obj[1].pos.y) - k] = ' ';
                if(p.mat[j->obj[1].pos.x][(j->obj[1].pos.y) + k] != '#')
                    p.mat[j->obj[1].pos.x][(j->obj[1].pos.y) + k] = ' ';
            }
    
    
    envia_jogo(j,p.n_ativos);
    //pthread_mutex_unlock(&explosao);
    pthread_exit(0);
}

void trata_jogo (Jogador *j,int n_jogadores,Jogador player){
        int i=0;

	//char mat[20][20];
	while(player.PID != j[i].PID)
		i++;
    
    
    //===================================== Movimento p/ Esquerda ==============================
	if(strcmp(player.p.comando,"esquerda") == 0){
		//if(j[i].pos.y > 1)
			if(p.mat[j[i].pos.x][(j[i].pos.y)-1] == ' ' || p.mat[j[i].pos.x][(j[i].pos.y)-1] == 'b' || p.mat[j[i].pos.x][(j[i].pos.y)-1] == 'B' || p.mat[j[i].pos.x][(j[i].pos.y)-1] == 'v' ||  p.mat[j[i].pos.x][(j[i].pos.y)-1] == 'F'){
                if(p.mat[j[i].pos.x][(j[i].pos.y)-1] == 'b')
                    j[i].obj[0].contObj++;
                if(p.mat[j[i].pos.x][(j[i].pos.y)-1] == 'B')
                    j[i].obj[1].contObj++;
                if(p.mat[j[i].pos.x][(j[i].pos.y)-1] == 'v')
                    j[i].vidas++;
                
                p.mat[j[i].pos.x][j[i].pos.y] = ' ';
				j[i].pos.y--;
                p.mat[j[i].pos.x][j[i].pos.y] = j[i].caracter;
				envia_jogo(j,n_jogadores);
			}

    //===================================== Movimento p/ Baixo ==============================
	}else if(strcmp(player.p.comando,"baixo") == 0){
		//if(j[i].pos.x < 19)
			if(p.mat[(j[i].pos.x)+1][j[i].pos.y] == ' ' || p.mat[(j[i].pos.x)+1][j[i].pos.y] == 'b' || p.mat[(j[i].pos.x)+1][j[i].pos.y] == 'B' || p.mat[(j[i].pos.x)+1][j[i].pos.y] == 'v' || p.mat[(j[i].pos.x)+1][j[i].pos.y] == 'F'){
                if(p.mat[(j[i].pos.x)+1][j[i].pos.y] == 'b')
                    j[i].obj[0].contObj++;
                if(p.mat[(j[i].pos.x)+1][j[i].pos.y] == 'B')
                    j[i].obj[1].contObj++;
                if(p.mat[(j[i].pos.x)+1][j[i].pos.y] == 'v')
                    j[i].vidas++;
                
                p.mat[j[i].pos.x][j[i].pos.y] = ' ';
				j[i].pos.x++;
                p.mat[j[i].pos.x][j[i].pos.y] = j[i].caracter;
				envia_jogo(j,n_jogadores);
			}



    //===================================== Movimento p/ Direita ==============================
	}else if(strcmp(player.p.comando,"direita") == 0){
		//if(j[i].pos.y < 59)
			if(p.mat[j[i].pos.x][(j[i].pos.y)+1] == ' ' || p.mat[j[i].pos.x][(j[i].pos.y)+1] == 'b' || p.mat[j[i].pos.x][(j[i].pos.y)+1] == 'B' || p.mat[j[i].pos.x][(j[i].pos.y)+1] == 'v' || p.mat[j[i].pos.x][(j[i].pos.y)+1] == 'F'){
                if(p.mat[j[i].pos.x][(j[i].pos.y)+1] == 'b')
                    j[i].obj[0].contObj++;
                if(p.mat[j[i].pos.x][(j[i].pos.y)+1] == 'B')
                    j[i].obj[1].contObj++;
                if(p.mat[j[i].pos.x][(j[i].pos.y)+1] == 'v')
                    j[i].vidas++;
                p.mat[j[i].pos.x][j[i].pos.y] = ' ';
				j[i].pos.y++;
                p.mat[j[i].pos.x][j[i].pos.y] = j[i].caracter;
				envia_jogo(j,n_jogadores);
			}



    //===================================== Movimento p/ Cima ==============================
	}else if(strcmp(player.p.comando,"cima") == 0){
		//if(j[i].pos.x > 1)
			if(p.mat[(j[i].pos.x)-1][j[i].pos.y] == ' ' || p.mat[(j[i].pos.x)-1][j[i].pos.y] == 'b' || p.mat[(j[i].pos.x)-1][j[i].pos.y] == 'B' || p.mat[(j[i].pos.x)-1][j[i].pos.y] == 'v' || p.mat[(j[i].pos.x)-1][j[i].pos.y] == 'F'){
                if(p.mat[(j[i].pos.x)-1][j[i].pos.y] == 'b')
                    j[i].obj[0].contObj++;
                if(p.mat[(j[i].pos.x)-1][j[i].pos.y] == 'B')
                    j[i].obj[1].contObj++;
                if(p.mat[(j[i].pos.x)-1][j[i].pos.y] == 'v')
                    j[i].vidas++;
                
                p.mat[j[i].pos.x][j[i].pos.y] = ' ';
				j[i].pos.x--;
                p.mat[j[i].pos.x][j[i].pos.y] = j[i].caracter;
				envia_jogo(j,n_jogadores);
			}



    //===================================== Bomba Pequena ==============================
	}else if(strcmp(player.p.comando,"bpequena") == 0){
        if(j[i].obj[0].contObj > 0){
            j[i].obj[0].pos.y = j[i].pos.y;
            j[i].obj[0].pos.x = j[i].pos.x;
            j[i].obj[0].tipo = 'b';
            j[i].obj[0].contObj--;
            
            pthread_t explosao_p;
            //pthread_mutex_init(&explosao, NULL); 
            pthread_create(&explosao_p, NULL, &cria_explosao_p, (void*)&j[i]);
            
            
        }


    //===================================== Bomba Grande ==============================
	}else if(strcmp(player.p.comando,"bgrande") == 0){
       if(j[i].obj[1].contObj > 0){
            j[i].obj[1].pos.y = j[i].pos.y;
            j[i].obj[1].pos.x = j[i].pos.x;
            j[i].obj[1].tipo = 'B';
            j[i].obj[1].contObj--;
           
           pthread_t explosao_g;
           //pthread_mutex_init(&explosao, NULL); 
           pthread_create(&explosao_g,NULL,&cria_explosao_g, (void*)&j[i]);
           
        }
	}




}

void *controla_inimigos (void* dados){
    Inimigo *inim;
    int i,ch,drop;
    Posicao pos;
    inim = (Inimigo*) dados;
    
    
    p.mat[inim->pos.x][inim->pos.y] = inim->id;
    
    do{
        ch = random_l_h(0,3); // escolhe uma direção random
        //printf("rand: %d\n",ch);
        
        pos.y = 0;
        pos.x = 0;
        
        
        if(ch == 0)  
            pos.x--;   // para cima
        if(ch == 1)
            pos.x++;   // para baixo
        if(ch == 2)
            pos.y--;   // para a esquerda
        if(ch == 3)
            pos.y++;   // para a direita
        
        
        
        pthread_mutex_lock(&inimigo); // mutex para andar
        
        if(p.mat[inim->pos.x][inim->pos.y] == '@'){  // SE FOR APANHADO POR UMA BOMBA
            drop = random_l_h(0,100);
            sleep(2);
            if(drop < 30){ // 30 de percentagem de calhar ele dropar uma bomba pequena
                p.mat[inim->pos.x][inim->pos.y] = 'b';
                
                printf("Inimigo morreu deixou bomba pequena\n");
            }
            if(drop >=30 && drop < 50){ // 20% para calhar de ele dropar uma bomba grande
                 p.mat[inim->pos.x][inim->pos.y] = 'B';
                
                printf("Inimigo morreu deixou bomba grande\n");
            }
            if(drop >=50 && drop < 90){ //40% para calhar de ele dropar uma vida
                p.mat[inim->pos.x][inim->pos.y] = 'v';
                
                printf("Inimigo morreu e deixou uma vida\n");
            }
            if(drop >= 90){ //10% para rebentar uma bomba surpresa gigante
                sleep(1);
                for(int k=1;k<=5;k++){
                if(p.mat[(inim->pos.x) - k][inim->pos.y] != '#')
                    p.mat[(inim->pos.x) - k][inim->pos.y] = '@';
                if(p.mat[(inim->pos.x) + k][inim->pos.y] != '#')
                    p.mat[(inim->pos.x) + k][inim->pos.y] = '@';
                if(p.mat[inim->pos.x][(inim->pos.y) - k] != '#')
                    p.mat[inim->pos.x][(inim->pos.y) - k] = '@';
                if(p.mat[inim->pos.x][(inim->pos.y) + k] != '#')
                    p.mat[inim->pos.x][(inim->pos.y) + k] = '@';
                }
                sleep(2);
                for(int k=1;k<=5;k++){
                if(p.mat[(inim->pos.x) - k][inim->pos.y] != '#')
                    p.mat[(inim->pos.x) - k][inim->pos.y] = ' ';
                if(p.mat[(inim->pos.x) + k][inim->pos.y] != '#')
                    p.mat[(inim->pos.x) + k][inim->pos.y] = ' ';
                if(p.mat[inim->pos.x][(inim->pos.y) - k] != '#')
                    p.mat[inim->pos.x][(inim->pos.y) - k] = ' ';
                if(p.mat[inim->pos.x][(inim->pos.y) + k] != '#')
                    p.mat[inim->pos.x][(inim->pos.y) + k] = ' ';
                }
                printf("Inimigo morreu e provocou uma explosao gigante\n");
            }
            
            pthread_mutex_unlock(&inimigo); // liberta o mutex do inimigo
            pthread_exit(0);
        }   
        if(p.mat[inim->pos.x + pos.x][inim->pos.y + pos.y] == ' '){
            //printf("e possivel\n");
            
            p.mat[inim->pos.x][inim->pos.y] = ' ';
            inim->pos.y += pos.y;
            inim->pos.x += pos.x;
            p.mat[inim->pos.x][inim->pos.y] = inim->id;
            
        }
        //printf("id: %d \nPos: %d | %d \n",inim->id,inim->pos.x,inim->pos.y);
        pthread_mutex_unlock(&inimigo); // liberta o mutex do inimigo
        usleep(200000);
    }while(FIM != 1); // termina a thread dele
    pthread_exit(0);
}


int main(int argc, char** argv) {
    char nomefich[20], mensagem[20],fifo_cliente[25],caracter = 49;
    Jogador player,*j;
    int fd,res,nfd,trata = 0,n_jogadores=0,verifica = -1,px,py,i;
    fd_set read_fds;
    Inimigo inim[nInim];
    
    struct timeval tv;
    union sigval val;
    
    srand(time(NULL));
    
    
    pthread_mutex_init(&inimigo, NULL); //cria mutex para os inimigos

    int x,y;
    
    //===================================== LABIRINTO ==============================
    //prepara labirinto
    for(x=0;x<L;x++)
        for(y=0;y<C;y++)
                p.mat[x][y] = ' ';

    //lados do labirinto
    for(x=0;x<L;x++)
        p.mat[x][0] = '#';

     for(y=0;y<C;y++)
        p.mat[0][y] = '#';

     for(x=0;x<L;x++)
        p.mat[x][60] = '#';

     for(y=0;y<C;y++)
        p.mat[20][y] = '#';

     
     //paredes indestrutiveis
     for(x=1;x<L-1;x++)
        for(y=1;y<C-1;y++)
            if(p.mat[x][y] == ' ')
                if(random_l_h(0,100) < 30)
                    p.mat[x][y] = '#';
    
     //paredes destrutiveis, bombas pequenas, bombas grandes e vidas
     for(x=1;x<L-1;x++)
        for(y=1;y<C-1;y++)
		    if(p.mat[x][y] != '#'){
                if(random_l_h(0,100) < 50)
                    p.mat[x][y] = '-';
                if(random_l_h(0,100) < 3)
                    p.mat[x][y] = 'b';
                if(random_l_h(0,100) < 1)
                    p.mat[x][y] = 'B';
                if(random_l_h(0,100) < 2)
                    p.mat[x][y] = 'v';
            }
                	
    //final do labirinto
     p.mat[L/2][C/2] = 'F';
    
    
    //===================================== THREADS INIMIGOS ==============================
    for(i=0;i<nInim;i++){
        inim[i].id = 'I';
        do{
            inim[i].pos.x = random_l_h(1,19); //arranja posicoes aleatorias para eles que estejam vazias
            inim[i].pos.y = random_l_h(1,59);
        }while((p.mat[inim[i].pos.x][inim[i].pos.y]) != ' ');
        pthread_create(&inimigos[i], NULL, &controla_inimigos, (void*) &inim[i]); //cria a thread
    }

    
    
    if(argc < 2){
        printf("Faltam argumentos.\n");
        return 0;
    }

    strcpy(nomefich,argv[1]);

    if (access(SERVER_FIFO, F_OK) != -1) {
        printf("Ja existe um Servidor em execucao!\n");
        exit(EXIT_FAILURE);
    }

    printf("--------- SERVIDOR DE JOGO --------- \n");

    //prepara_labirinto(&p);

    if(signal(SIGINT,trataSig) == SIG_ERR){
      perror("\nNao foi possivel configurar o sinal SIGINT\n");
      exit (EXIT_FAILURE);
    }

    res = mkfifo(SERVER_FIFO,0777);
    if(res == -1){
      perror("Erro ao criar FIFO do Servidor.\n");
      exit(EXIT_FAILURE);
    }

    //printf("\nFIFO servidor criado\n");

    s_fifo_fd = open(SERVER_FIFO, O_RDWR);
    if(s_fifo_fd == -1){
      perror("Erro ao abrir o FIFO do servidor\n");
      exit(EXIT_FAILURE);
    }

	
    
    
    //===================================== CICLO PRINCIPAL ==============================
    while(1){
      tv.tv_sec = 1; // tempo do timeout em segundos
      tv.tv_usec = 0; //  ..         ..   em milisegundos

      FD_ZERO(& read_fds);
      FD_SET(0, & read_fds);
      FD_SET(s_fifo_fd, &read_fds);
      nfd = select( s_fifo_fd + 1, & read_fds, NULL, NULL, & tv);

      if(nfd == 0){
        fflush(stdout);
          /*
          for (px = 0; px < 21; px++){
           for (py = 0; py < 61; py++){
                printf("%c", p.mat[px][py]);

            }
            printf("\n");
             }
          */
          //faz várias verificações
          
          for(i=0;i<n_jogadores;i++){
            if(j[i].pos.x == L/2 && j[i].pos.y == C/2){ //Se chegar à posição final acaba tudo e envia sinal a todos os clientes e o servidor termina
                val.sival_int = 1;
                for(int k=0; k < n_jogadores; k++){
                    sigqueue(j[k].PID, SIGUSR2, val); //Envia Sinal para todos os clientes
                }
                
                FIM = 1; // para as threads pararem
                
                for(int k=0; k < nInim;k++){
                    pthread_join(inimigos[k],NULL);   //DESTROI AS THREADS 
                }

                close(s_fifo_fd);
                unlink(SERVER_FIFO);
                return (EXIT_SUCCESS);
            }
              // Verifica se está algum inimigo à sua volta e retira uma vida
            if(p.mat[(j[i].pos.x) - 1][j[i].pos.y] == 'I' ||
               p.mat[(j[i].pos.x) + 1][j[i].pos.y] == 'I' ||
               p.mat[j[i].pos.x][(j[i].pos.y) - 1] == 'I' ||
               p.mat[j[i].pos.x][(j[i].pos.y) + 1] == 'I'){
                j[i].vidas--;
            }
              // Verifica se foi apanhado por uma bomba e retira uma vida por iteração
              // se não mexer o boneco tira 2 vidas -> sleep 2 segundos
            if(p.mat[j[i].pos.x][j[i].pos.y] == '@'){
                j[i].vidas--;
            }
              // verifica se já não tem vidas
            if(j[i].vidas <=0){
                sleep(2);  //espera que a explosão acabe
                p.mat[j[i].pos.x][j[i].pos.y] = ' ';
                //EXPULSA DO ARRAY DE JOGADORES E MANDA SINAL PARA ELE NA FUNC
                j = expulsaJogador (j[i].username,j,n_jogadores,&verifica);
		        if(verifica != -1)
			        n_jogadores = n_jogadores - 1;    
                
            }
          }
          // esta constantemente a enviar o labirinto a todos os jogadores, mesmo que o servidor não receba nada
        envia_jogo(j,n_jogadores);
        continue;
      }
      if(nfd == -1){
        printf("Erro no select\n");
        close(s_fifo_fd);
        unlink(SERVER_FIFO);
        return 0;
      }
	

      
    


      if(FD_ISSET(0,& read_fds)){
          tratateclado(nomefich,j,&n_jogadores); // Comandos do ADMIN
      }
      if(FD_ISSET(s_fifo_fd,& read_fds)){

          read(s_fifo_fd, &player, sizeof(player)); // Le do seu pipe
	  //se tiver logado
	
	  if(player.login == 1){ 
		  //envia_jogo(j,n_jogadores);
          //printf("Pedido de : %d",player.PID);
          // printf("Posicoes recebidas do cliente %d | x: %d | y: %d\n",player.PID,player.pos.x,player.pos.y);
	  	  trata_jogo(j,n_jogadores,player); // trata de todas funcionalidades logicas do jogo
	  }
	  else
		if(player.login == -1){ // se receber o login a -1, procura o id dele e expulsa-o
            i=0;
            while(player.PID != j[i].PID)
                i++;
            p.mat[j[i].pos.x][j[i].pos.y] = ' ';
			j = expulsaJogador (player.username,j,n_jogadores,&verifica);
			if(verifica != -1)
				n_jogadores = n_jogadores - 1;
            envia_jogo(j,n_jogadores);
		  }else
		    if(verificaLogin(nomefich,player.username,player.password,j,n_jogadores) == 1){
             // se conseguir entrar no jogo

		    n_jogadores+=1;
		    j = criaVetor(j,n_jogadores);

		    j[n_jogadores - 1] = player;

		    j[n_jogadores - 1].login = 1;
		    j[n_jogadores - 1].caracter = caracter++;
            j[n_jogadores - 1].obj[0].contObj = 4;
            j[n_jogadores - 1].obj[1].contObj = 2;
            j[n_jogadores - 1].vidas = 3;
            j[n_jogadores - 1].pontos = 0;
                
                // Gera posições random para o jogador, por vezes pode bugar devido ao mapa ser random e poder calhar numa zona fechada
            
                do{
                
                    j[n_jogadores - 1].pos.x = random_l_h(1,19);
                    j[n_jogadores - 1].pos.y = random_l_h(1,59);
              
                   // printf("x: %d\n",j[n_jogadores - 1].pos.x);
                    
                }while(p.mat[j[n_jogadores - 1].pos.x][j[n_jogadores - 1].pos.y] != ' '); 
		        
                p.j[n_jogadores - 1] = j[n_jogadores - 1];
                //p.mat[j[n_jogadores - 1].pos.x][j[n_jogadores - 1].pos.y] = j[n_jogadores - 1].caracter;

		    sprintf(fifo_cliente,CLIENT_FIFO,player.PID);
		    c_fifo_fd = open (fifo_cliente, O_WRONLY);
		    if(c_fifo_fd == -1)
		       printf("Erro ao abrir FIFO do Cliente [%d].\n",player.PID);
		    
		    res = write(c_fifo_fd, &p,sizeof(Partida));
		    if(res != sizeof(Partida))
		       perror("Erro ao enviar resposta ao Cliente\n");
		    close (c_fifo_fd);
		  }
		  else{
		    sprintf(fifo_cliente,CLIENT_FIFO,player.PID);
		    c_fifo_fd = open (fifo_cliente, O_WRONLY);
		    if(c_fifo_fd == -1)
		       printf("Erro ao abrir FIFO do Cliente [%d].\n",player.PID);
		    player.login = 0;
		    res = write(c_fifo_fd, &player,sizeof(Jogador));
		    if(res != sizeof(Jogador))
		       perror("Erro ao enviar resposta ao Cliente\n");
		    close (c_fifo_fd);
		  }

      }
    } // FIM DO CICLO


    close(s_fifo_fd);
    unlink(SERVER_FIFO);

    return (EXIT_SUCCESS);
}
