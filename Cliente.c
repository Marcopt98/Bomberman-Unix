#include "Estruturas.h"

static volatile int sinal_ctrl = 0;

void trataSig(int i){
    printf("\n Cliente vai terminar via Ctrl+C\n\n");
    sinal_ctrl = 1;
}
//===================================== RECEBE SINAIS DO SERVIDOR ==============================
void recebesinal(int s, siginfo_t *dados, void *context){
    char buffer[50];
    int pid;
    pid = getpid();

    sprintf(buffer, CLIENT_FIFO, pid);

    if (s == SIGUSR1) {
        
        mvprintw(40,50,"Foi expulso do jogo pelo administrador!\n");
        unlink(buffer);
        sleep(1);
	    endwin();
        exit(EXIT_SUCCESS);
    }

    if (s == SIGUSR2) {
        printf("O Servidor foi encerrado!\n");
        unlink(buffer);
        sleep(1);
	    endwin();
        exit(EXIT_SUCCESS);
    }
}

//===================================== MOSTRA LABIRINTO E AS INFOS DO JOGADOR ==============================
void  mostra_labirinto(Partida p){
    int i,px,py,pid;
    
    pid=getpid();
    i=0;
    while(p.j[i].PID != pid) // encontra a posição do jogador na struct
        i++;
    
    	
	refresh();
    wclear(win_game); ///limpa janela
    wprintw(win_game,"\n-------------------------------------------------------------\n");
	wprintw(win_game,"\n		      	    JOGO			    \n");
	wprintw(win_game,"\n-------------------------------------------------------------\n");
    wprintw(win_game,"  Nome: %s | Vidas: %d | Bombas grandes: %d | Bombas pequenas: %d\n",p.j[i].username,p.j[i].vidas,p.j[i].obj[1].contObj,p.j[i].obj[0].contObj);
    wprintw(win_game,"\n\n");
    wrefresh(win_game); ///da refresh na janela
	
	

    

   


    ///impressao da p.matriz compelta na janela do jogo
    
   for (px = 0; px < 21; px++){
       for (py = 0; py < 61; py++){
            wprintw(win_game,"%c", p.mat[px][py]);
            wrefresh(win_game);
        }
        wprintw(win_game,"\n");
        wrefresh(win_game);
    }
    
    
    
}

//===================================== MAIN ==============================
int main(int argc, char** argv) {
    int s_fifo_fd;
    int c_fifo_fd;
    Jogador player;
    char fifo_cliente[25]; // nome do fifo deste cliente
    int op=0,login=0,read_res,x=0,y=0,ch,nfd,i=0;
    fd_set read_fds;
    struct sigaction sinal;
    struct timeval tv;
    Partida p;
    
    

    //===================================== NCURSES ==============================
    initscr(); //incializa o ecra
    start_color();
    raw();
    cbreak();
    echo(); //ecoa teclas quando sao introduzidas
    keypad(stdscr,TRUE);
    curs_set(0);
    clear();
    

    win_menu=newwin(40,85,3,40);///criacao da janela para menu
    scrollok(win_menu,TRUE); // com scroll ativo
    win_game=newwin(40,85,3,40); ///criacao da janela para jogo
    init_pair(1,COLOR_BLACK,COLOR_BLACK);
    //init_pair(2,COLOR_BLUE,COLOR_WHITE);
    
    wbkgd(stdscr, COLOR_PAIR(1));
    //wbkgd(win_menu,COLOR_PAIR(2));    

    //===================================== TRATAMENTO DOS SINAIS ==============================
    memset(&sinal, 0,sizeof(sinal));
    sinal.sa_flags = SA_SIGINFO;
    sinal.sa_sigaction = &recebesinal;

    sigaction (SIGUSR1, &sinal, NULL);
    sigaction (SIGUSR2, &sinal, NULL);

    player.PID = getpid();
	
    //strcpy(player.p.comando,"nada");
    sprintf(fifo_cliente, CLIENT_FIFO, player.PID);

    if(signal(SIGINT,trataSig) == SIG_ERR){
      wprintw(win_menu,"\nNao foi possivel configurar o sinal SIGINT\n");
      endwin();
      exit (EXIT_FAILURE);
    }

    if(mkfifo(fifo_cliente, 0777) == -1){
      wprintw(win_menu,"Erro ao criar o fifo do Cliente.\n");
      endwin();
      return 0;
    }

    // abre o fifo do servidor para escrita
    s_fifo_fd = open (SERVER_FIFO, O_WRONLY);
    if(s_fifo_fd == -1){
	wprintw(win_menu,"O servidor nao esta a correr!\n");
        wrefresh(win_menu);
        sleep(1);
        endwin();
        unlink(fifo_cliente);
      return 0;
    }


    c_fifo_fd = open (fifo_cliente, O_RDWR);

    if(c_fifo_fd == -1){
        wprintw(win_menu,"Erro ao abrir o FIFO do Cliente\n");
	endwin();
        close(s_fifo_fd);
        unlink(fifo_cliente);
        exit(EXIT_FAILURE);
    }




    do{

      

  
	  wprintw(win_menu,"  ================================================================================\n");
	  wprintw(win_menu,"  |                                   BOMBERMAN                                  |\n");
	  wprintw(win_menu,"  |                                                                              |\n");
	  wprintw(win_menu,"  |                                                                              |\n");
	  wprintw(win_menu,"  |                                            ###                               |\n");
      wprintw(win_menu,"  |                                            ###                               |\n");
      wprintw(win_menu,"  |                                             #                                |\n");
      wprintw(win_menu,"  |                                            #                                 |\n");
      wprintw(win_menu,"  |                                           #                                  |\n");
      wprintw(win_menu,"  |                                          #                                   |\n");
      wprintw(win_menu,"  |                                         #                                    |\n");
      wprintw(win_menu,"  |                                   ############                               |\n");
      wprintw(win_menu,"  |                                 ################                             |\n");
      wprintw(win_menu,"  |                                ###########   ####                            |\n");
      wprintw(win_menu,"  |                               ###########     ####                           |\n");
      wprintw(win_menu,"  |                              #############   ######                          |\n");
      wprintw(win_menu,"  |                              ######################                          |\n");
      wprintw(win_menu,"  |                               ####################                           |\n");
      wprintw(win_menu,"  |                                ##################                            |\n");
      wprintw(win_menu,"  |                                 ################                             |\n");
      wprintw(win_menu,"  |                                   ############                               |\n");
      wprintw(win_menu,"  ================================================================================\n");
	  wprintw(win_menu,"                                       MENU                                 \n\n");
	  wprintw(win_menu,"  1 - Jogar                           \n");
	  wprintw(win_menu,"  2 - Sair                            \n");
	  
	    
	  wrefresh(win_menu);
      //m = wmenu();  
                            
      //getyx(m, y, x);
      wprintw(win_menu," -> "); 
      wscanw(win_menu," %d",&op); 
      


      switch (op) {
        case 1:
                do{
  		            player.login=0;
                    
		    
		            wprintw(win_menu,"\n  ================================================================================\n");
  		            wprintw(win_menu,"                                      LOGIN                                     \n");
 		            wprintw(win_menu,"  ================================================================================\n");
		            wprintw(win_menu," ('sair' para voltar ao menu inicial!)\n");
                    wprintw(win_menu,"\n Username: ");
                    wscanw(win_menu," %s",player.username);
		
                    fflush(stdout);
                    if(strcmp(player.username,"sair")==0)
                        break;
                    wprintw(win_menu,"\n Password: ");
                    wscanw(win_menu," %s",player.password);
                    fflush(stdout);
                    write(s_fifo_fd, &player, sizeof(Jogador));
                    read(c_fifo_fd, &p, sizeof(Partida));
                    
                    while(player.PID != p.j[i].PID)
                         i++;
                    
                    if(p.j[i].login == 1){
                        player.login = 1;
                        //player.pos.x = p.j[i].pos.x;
                        //player.pos.y = p.j[i].pos.y;
                        wprintw(win_menu,"|%s entrou com sucesso!|\n",player.username);

                        wrefresh(win_menu);
                        sleep(1);
                        wclear(win_menu);


                        mostra_labirinto(p);


                        while(1){

                                tv.tv_sec = 1; // tempo do timeout em segundos
                                tv.tv_usec = 0; //  ..         ..   em milisegundos

                                FD_ZERO(&read_fds);
                                FD_SET(0, &read_fds);
                                FD_SET(c_fifo_fd, &read_fds);
                                nfd = select( c_fifo_fd + 1, & read_fds, NULL, NULL, & tv);

                                if(nfd == 0){
                                      fflush(stdout);
                                      mostra_labirinto(p);
                                        if(sinal_ctrl == 1){
                                              player.login = -1;
                                              write(s_fifo_fd, &player, sizeof(Jogador));
                                              close(c_fifo_fd);
                                              close(s_fifo_fd);
                                              unlink(fifo_cliente);
                                              endwin();
                                              exit(EXIT_SUCCESS);
                                        }

                                      continue;
                                }

                                if(nfd == -1){
                                      //printf("Erro no select\n");
                                      close(s_fifo_fd);
                                      unlink(fifo_cliente);
                                      return 0;
                                }



                                if(FD_ISSET(0,&read_fds)){
                                //===================================== LABIRINTO ==============================
                                ch = getch();
                                    // ESC para sair!
                                    if(ch == ESC){
                                        player.login = -1;
                                        write(s_fifo_fd, &player, sizeof(Jogador));
                                        close(c_fifo_fd);
                                        close(s_fifo_fd);
                                        unlink(fifo_cliente);
                                        endwin();
                                    }

                                    switch(ch){
                                        case 'w':
                                            strcpy(player.p.comando,"cima");
                                            write(s_fifo_fd,&player,sizeof(Jogador));
                                            break;
                                        case 'a':
                                            strcpy(player.p.comando,"esquerda");
                                            write(s_fifo_fd,&player,sizeof(Jogador));
                                            break;
                                        case 's':
                                            strcpy(player.p.comando,"baixo");
                                            write(s_fifo_fd,&player,sizeof(Jogador));
                                            break;
                                        case 'd':
                                            strcpy(player.p.comando,"direita");
                                            write(s_fifo_fd,&player,sizeof(Jogador));
                                            break;
                                        case 'n':
                                            strcpy(player.p.comando,"bgrande");
                                            write(s_fifo_fd,&player,sizeof(Jogador));
                                            //põe a bomba grande
                                            break;
                                        case 'm':
                                            strcpy(player.p.comando,"bpequena");
                                            write(s_fifo_fd,&player,sizeof(Jogador));
                                            //poe a bomba pequena
                                            break;
                                    }

                                }

                                if(FD_ISSET(c_fifo_fd, &read_fds)){
                                    read(c_fifo_fd, &p, sizeof(Partida));// está sempre a ler do servidor e a mostrar o lab.

                                    mostra_labirinto(p);
                                }
                        }

                        }
                        else
                          if(player.login == -2){
                            wprintw(win_menu,"\nO Jogo já está cheio!\n");
                            break;
                              }else{
                            wprintw(win_menu,"Username ou password incorretos.\n");
                                //sleep(2);
                          }


                }while (player.login!=1);
                break;
          case 2:
	              
	            wprintw(win_menu,"Saiu!\n");
                break;

        default:
      	        wprintw(win_menu,"\nInsira apenas valores validos\n");
      	        op=3;
        	break;
            }
	
	wrefresh(win_menu);
    }while(op!=2);

    close(c_fifo_fd);
    close(s_fifo_fd);
    unlink(fifo_cliente);

    werase(win_menu);
    delwin(win_menu);
    endwin();

    return (EXIT_SUCCESS);
}
