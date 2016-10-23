/*
// Projeto SO - exercicio 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2016-17
*/

/*
// Projecto realizado por:
// Pedro Caetano, N:56564
// Ricardo Almeida, N:63108
*/

#include "commandlinereader.h"
#include "contas.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>



#define COMANDO_DEBITAR "debitar"
#define COMANDO_CREDITAR "creditar"
#define COMANDO_LER_SALDO "lerSaldo"
#define COMANDO_SIMULAR "simular"
#define COMANDO_SAIR "sair"
#define ARGUMENTO_AGORA "agora"

#define MAXARGS 3
#define BUFFER_SIZE 100


/*
// Prototipos
*/
void apanhaUSR1(int s);


int main (int argc, char** argv) {

    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
    int nProcessos=0; /* guarda o numero de processos já criados*/
    
    inicializarContas();
	sinalRecebido=0;
	
	if(signal(SIGUSR1,apanhaUSR1)==SIG_ERR){
		perror("ERRO ao criar tratamento de sinal");
		exit(EXIT_SUCCESS);
		}
		
	
    printf("Bem-vinda/o ao i-banco\n\n");
      
    while (1) {
        int numargs;
    
        numargs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);

        /* EOF (end of file) do stdin ou comando "sair" */
        if (numargs < 0 ||
	        (numargs > 0 && (strcmp(args[0], COMANDO_SAIR) == 0))) {
            int i, pid,status;
        
                if(numargs>1 && strcmp(args[1], ARGUMENTO_AGORA) == 0){
                    if (kill(0,SIGUSR1)<0){
                        printf("%s %s: ERRO",COMANDO_SAIR,ARGUMENTO_AGORA);
						exit(EXIT_FAILURE);
					}
                }
				
                printf("i-banco vai terminar.\n");
                printf("--\n");
				
                for(i=0;i<nProcessos;i++){
					pid=wait(&status);
					/*error on wait*/
					if(pid<0){
						
                        if (errno==EINTR){
                            i--;
                            continue;
                        }
                            perror("Error na funcao wait.");
                            exit(EXIT_FAILURE);
					}
                    if(WIFEXITED(status))
						printf("FILHO TERMINADO (PID=%d; terminou normalmente)\n",pid);
					else
						printf("FILHO TERMINADO (PID=%d; terminou abruptamente)\n",pid);
		}
            printf("--\n");
			printf("i-banco terminou.\n\n");
            exit(EXIT_SUCCESS);
        }
    
        else if (numargs == 0)
            /* Nenhum argumento; ignora e volta a pedir */
            continue;
            
        /* Debitar */
        else if (strcmp(args[0], COMANDO_DEBITAR) == 0) {
            int idConta, valor;

            if (numargs < 3) {
                printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_DEBITAR);
	           continue;
            }

            idConta = atoi(args[1]);
            valor = atoi(args[2]);

            if (debitar (idConta, valor) < 0)
	           printf("%s(%d, %d): Erro\n\n", COMANDO_DEBITAR, idConta, valor);
            else
                printf("%s(%d, %d): OK\n\n", COMANDO_DEBITAR, idConta, valor);
    }

    /* Creditar */
    else if (strcmp(args[0], COMANDO_CREDITAR) == 0) {
        int idConta, valor;
        
        if (numargs < 3) {
            printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_CREDITAR);
            continue;
        }

        idConta = atoi(args[1]);
        valor = atoi(args[2]);

        if (creditar (idConta, valor) < 0)
            printf("%s(%d, %d): Erro\n\n", COMANDO_CREDITAR, idConta, valor);
        else
            printf("%s(%d, %d): OK\n\n", COMANDO_CREDITAR, idConta, valor);
    }

    /* Ler Saldo */
    else if (strcmp(args[0], COMANDO_LER_SALDO) == 0) {
        int idConta, saldo;

        if (numargs < 2) {
            printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_LER_SALDO);
            continue;
        }
        idConta = atoi(args[1]);
        saldo = lerSaldo (idConta);
        if (saldo < 0)
            printf("%s(%d): Erro.\n\n", COMANDO_LER_SALDO, idConta);
        else
            printf("%s(%d): O saldo da conta é %d.\n\n", COMANDO_LER_SALDO, idConta, saldo);
    }

    /* Simular */
    else if (strcmp(args[0], COMANDO_SIMULAR) == 0) {
		int anos,pid;
		
		
		if (numargs < 2) {
			printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_SIMULAR);
			continue;
		}
            
        anos = atoi(args[1]);
        
            /*Cria o processo*/
            pid=fork();
            
            if(pid<0){
				perror("Não foi possivel criar o processo \n");
                continue;
            }
            
			if (pid==0){
				simular(anos);
				exit(EXIT_SUCCESS);
			}
			else{
				nProcessos++;
				continue;
			}
			     
      }
	
	/*Comando desconhecido*/
    else{
      printf("Comando desconhecido. Tente de novo.\n");
    }

  } 
}

void apanhaUSR1(int s){
		if(signal(SIGUSR1,apanhaUSR1)==SIG_ERR){
			perror("ERRO ao criar tratamento de sinal");
			exit(EXIT_SUCCESS);
		}
	sinalRecebido=1;
	}

