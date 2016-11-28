#include "commandlinereader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


#define COMANDO_DEBITAR "debitar"
#define COMANDO_CREDITAR "creditar"
#define COMANDO_TRANSFERIR "transferir"
#define COMANDO_LER_SALDO "lerSaldo"
#define COMANDO_SIMULAR "simular"
#define COMANDO_SAIR "sair"
#define COMANDO_SAIR_TERMINAL "sair-terminal"
#define ARGUMENTO_AGORA "agora"

#define COMANDO_DEBITAR_ID 1
#define COMANDO_CREDITAR_ID 2
#define COMANDO_TRANSFERIR_ID 6
#define COMANDO_LER_SALDO_ID 3
#define COMANDO_SIMULAR_ID 4
#define COMANDO_SAIR_ID 5
#define COMANDO_SAIR_AGORA_ID 7

#define MAXARGS 4
#define BUFFER_SIZE 100

typedef struct{
  int PID;
  int operacao;
  int idConta;
  int valor;
  int idContaDestino;
        
} comando_t;

int pipeEscrita,
  pipeLeitura;


void adicionarComando(int Comando, int idConta, int valor, int idContaDestino);

int main (int argc, char** argv) {
  char *args[MAXARGS + 1];
  char buffer[BUFFER_SIZE];
  
  if(argc>1){
    openPipeEscrita(argv[1]);
  }
  
  printf("Bem-vinda/o ao i-banco\n\n");

  while (1) {
    int numargs;
    
    numargs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);


    
    /* Comando sair (ou EOF (end of file) do stdin )*/
    
    if (numargs < 0 ||
      (numargs > 0 && (strcmp(args[0], COMANDO_SAIR_TERMINAL) == 0))) {
                      
        
      printf("i-banco-terminal vai terminar.\n");
      printf("--\n");
      printf("--\n");
      printf("i-banco-terminal terminou.\n\n");
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
      adicionarComando(COMANDO_DEBITAR_ID,idConta,valor,0);

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
      adicionarComando(COMANDO_CREDITAR_ID,idConta,valor,0);
    }

    /* Ler Saldo */
    else if (strcmp(args[0], COMANDO_LER_SALDO) == 0) {
      int idConta;

      if (numargs < 2) {
        printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_LER_SALDO);
        continue;
      }
      idConta = atoi(args[1]);
      adicionarComando(COMANDO_LER_SALDO_ID,idConta,0,0);
    }
  
    /* Transferir */
    else if (strcmp(args[0], COMANDO_TRANSFERIR) == 0) {
      int idConta, idContaDestino,valor;

      if (numargs < 4) {
        printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_TRANSFERIR);
        continue;
      }

      idConta = atoi(args[1]);
      idContaDestino = atoi(args[2]);
      valor = atoi(args[3]);
  
      adicionarComando(COMANDO_TRANSFERIR_ID,idConta,valor,idContaDestino);
    }

  
    /* Simular */
    else if (strcmp(args[0], COMANDO_SIMULAR) == 0) {
      int anos;

      if (numargs < 2) {
          printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_SIMULAR);
          continue;
      }

      anos = atoi(args[1]);
      adicionarComando(COMANDO_SIMULAR_ID,0,anos,0);

    }

    /* Simular */
    else if (strcmp(args[0], COMANDO_SAIR) == 0) {

      if(numargs>1 && strcmp(args[1], ARGUMENTO_AGORA) == 0)
        adicionarComando(COMANDO_SAIR_AGORA_ID,0,0,0);
      else
        adicionarComando(COMANDO_SAIR_ID,0,0,0);

      

    }
/*Comando desconhecido*/
    else{
      printf("Comando desconhecido. Tente de novo.\n");
    }
  }
}

void adicionarComando(int Comando, int idConta, int valor, int idContaDestino){
  comando_t cmd;
  cmd.operacao=(Comando);
  cmd.idConta=(idConta);
  cmd.valor=(valor);
  cmd.idContaDestino=(idContaDestino);
  cmd.PID=getpid();
  /*escreve o comando para o pipe*/
  write(pipeEscrita,&cmd,sizeof(comando_t));

}

void openPipeEscrita(char* file){
  if((pipeEscrita=open(file,O_WRONLY))<0){
    perror("não foi possivel abrir o ficheiro");
    exit(EXIT_SUCCESS);
  }
}