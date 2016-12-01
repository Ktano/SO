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
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>




#define COMANDO_DEBITAR "debitar"
#define COMANDO_CREDITAR "creditar"
#define COMANDO_TRANSFERIR "transferir"
#define COMANDO_LER_SALDO "lerSaldo"
#define COMANDO_SIMULAR "simular"
#define COMANDO_SAIR "sair"
#define COMANDO_SAIR_TERMINAL "sair-terminal"
#define ARGUMENTO_AGORA "agora"


#define READ_PIPE "/tmp/i-banco-pipe"
#define LOG_FILE "log.txt"

#define COMANDO_DEBITAR_ID 1
#define COMANDO_CREDITAR_ID 2
#define COMANDO_TRANSFERIR_ID 6
#define COMANDO_LER_SALDO_ID 3
#define COMANDO_SIMULAR_ID 4
#define COMANDO_SAIR_ID 5
#define COMANDO_SAIR_AGORA_ID 7


#define MAX_SIMULAR_NAME 40
#define MAX_PIPE_NAME 30

#define NUM_TRABALHADORAS 3
#define CMD_BUFFER_DIM (NUM_TRABALHADORAS * 2)


/* Estrutura de um comando*/
typedef struct{
  char pipeName[MAX_PIPE_NAME];
  int operacao;
  int idConta;
  int valor;
  int idContaDestino;
        
} comando_t;


/*
// Variáveis globais
*/
comando_t cmd_buffer[CMD_BUFFER_DIM];
int buff_write_idx = 0,
buff_read_idx = 0,
num_comandos=0;
pthread_t tid[NUM_TRABALHADORAS];
sem_t semLeitura,semEscrita;
pthread_mutex_t bufferReadMutex,cmdsMutex;
pthread_cond_t podeSimular;
int logFile,readPipe;


/*
// Prototipos
*/
void apanhaUSR1(int s);
void inicializarTarefas();
void *trabalhadora();
void adicionarComando(comando_t cmd);
void initSemRead();
void initSemWrite();
void SemPost(sem_t* sem);
void SemWait(sem_t* sem);
void initMutexRead();
void ReadLock();
void ReadUnlock();
void cmdInit();
void cmdLock();
void cmdUnlock();
void initCond();
void waitPodeSimular();
void signalPodeSimular();

void open_log();
void close_log();
void makePipe();
comando_t readCommand();
void writeResult(int res, char* pipe);


/* main*/

int main (int argc, char** argv) {

  int nProcessos=0; /* guarda o numero de processos já criados*/

  inicializarContas();
  inicializarTarefas();
  sinalRecebido=0;
  makePipe();
  
  /*cria vector para tratamento do sinal SIGUSR1*/
  if(signal(SIGUSR1,apanhaUSR1)==SIG_ERR){
    perror("ERRO ao criar tratamento de sinal");
    exit(EXIT_SUCCESS);
  }

  printf("Bem-vinda/o ao i-banco\n\n");

  open_log();
  
  while (1) {
    comando_t cmd;
    
    cmd=readCommand();
    
    
    /* Comando sair (ou EOF (end of file) do stdin )*/
    
    if (cmd.operacao==COMANDO_SAIR_ID || cmd.operacao==COMANDO_SAIR_AGORA_ID){
      int i, pid,status;
      
      /*se for para sair agora (manda signal)*/    
        if(cmd.operacao==COMANDO_SAIR_AGORA_ID){
          if (kill(0,SIGUSR1)<0){
            printf("Erro ao enviar sinal aos processos sair");
            exit(EXIT_FAILURE);
          }
        }
          
          
      printf("i-banco vai terminar.\n");
      printf("--\n");
      /*Terminates all threads*/
      for(i=0;i<NUM_TRABALHADORAS;i++)
        adicionarComando(cmd);

      for(i=0;i<nProcessos;i++){
        pid=wait(&status);
        /*error on wait*/
        if(pid<0){
          if (errno==EINTR){
            i--;
            continue;
          }
          perror("Error na funcao wait.\n");
          exit(EXIT_FAILURE);
        }

        if(WIFEXITED(status))
          printf("FILHO TERMINADO (PID=%d; terminou normalmente)\n",pid);
        else
          printf("FILHO TERMINADO (PID=%d; terminou abruptamente)\n",pid);
      }

      /*Confirms all threads are terminated*/
      for(i=0;i<NUM_TRABALHADORAS;i++)
          pthread_join(tid[i],NULL);

      printf("--\n");
      printf("i-banco terminou.\n\n");
      close_log();
      exit(EXIT_SUCCESS);
    }

    /* Creditar|| Debitar || lerSaldo || transferir */
    else if (cmd.operacao==COMANDO_DEBITAR_ID ||
      cmd.operacao==COMANDO_CREDITAR_ID ||
      cmd.operacao==COMANDO_LER_SALDO_ID||
      cmd.operacao==COMANDO_TRANSFERIR_ID) {
      
        adicionarComando(cmd);
    }

    /* Simular */
    else if (cmd.operacao == COMANDO_SIMULAR_ID) {
      int anos,pid;

      anos = cmd.valor;
      /*Teste se existem comandos a executar*/
      cmdLock();
      while(num_comandos!=0) waitPodeSimular();
          
      /*Cria o processo*/
      pid=fork();
      
      /*se processo nao foi criado*/
        if(pid<0){
          perror("Não foi possivel criar o processo \n");
          continue;
        }
      /*processo criado*/
      
      if (pid==0){
        /*filho*/
        int fp;
    
        /*Variável para nome do ficheiro para*/
        char name[MAX_SIMULAR_NAME]; 
        
        snprintf(name,MAX_SIMULAR_NAME, "i-banco-sim-%d.txt",getpid());

        
        if((fp = open(name,O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR))<0){
            perror("Erro criar ficheiro.\n");
            exit(EXIT_SUCCESS);
        };
        
        logFile=-1; /*torna o file descriptor do log invalido*/
        dup2(fp,1);
        simular(anos);
        cmdUnlock();
        if(close(fp)<0){
            perror("erro ao fechar o ficheiro");
            exit(EXIT_SUCCESS);
        }
        exit(EXIT_SUCCESS);
      }
      else{
        nProcessos++;
        cmdUnlock();
        continue;
      }
    }
    
    else{
      /*Comando invalido volta a tentar*/
      continue;
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

void inicializarTarefas(){
        int i=0,pthread;

    initMutexRead();
    cmdInit();
        initSemRead();
        initSemWrite();
        initCond();

        for(i=0;i<NUM_TRABALHADORAS;i++){
                pthread=pthread_create(&tid[i],0,trabalhadora,NULL);
                if(pthread!=0){
                        fprintf(stderr,"Erro ao criar thread\n");
                        exit(EXIT_FAILURE);
                }
        }
}



 
    /*log*/

void open_log(){
   
    logFile=open(LOG_FILE,O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR);
    
    if (logFile <0){
        perror("Erro ao abrir o log.\n");
        exit(EXIT_SUCCESS);
    }
    
}

void close_log(){
    
    if(close(logFile)<0){
     perror("Erro ao fechar o log.\n");
     exit(EXIT_SUCCESS);
    }
}

/*trabalhadora*/

void *trabalhadora(){
  comando_t cmd;
  
  while(1){
    SemWait(&semLeitura);
    ReadLock();

    cmd=cmd_buffer[buff_read_idx];
    buff_read_idx=(buff_read_idx+1)%CMD_BUFFER_DIM;

    ReadUnlock();
    SemPost(&semEscrita);

    /*Debitar*/
    if(cmd.operacao==COMANDO_DEBITAR_ID){
      writeResult(debitar (cmd.idConta, cmd.valor),cmd.pipeName);
    }

    /*Creditar*/
    else if(cmd.operacao==COMANDO_CREDITAR_ID){
      writeResult(creditar (cmd.idConta, cmd.valor),cmd.pipeName);
    }
    
    /*Transferir*/
    else if(cmd.operacao==COMANDO_TRANSFERIR_ID){
      writeResult(transferir(cmd.idConta, cmd.valor, cmd.idContaDestino),cmd.pipeName);
    }
    
    /*Ler Saldo*/
    else if(cmd.operacao==COMANDO_LER_SALDO_ID){
      writeResult(lerSaldo (cmd.idConta),cmd.pipeName);
    }
    /*Sair*/
    else if(cmd.operacao==COMANDO_SAIR_ID || cmd.operacao==COMANDO_SAIR_AGORA_ID){
            pthread_exit(0);
    }
    /*Decrementa o número de comandos a executar e sinaliza que é possivel simular*/
    cmdLock();
    if(--num_comandos==0){
      signalPodeSimular();
    }
    cmdUnlock();
  }
}

void adicionarComando(comando_t cmd){
        SemWait(&semEscrita);
        cmd_buffer[buff_write_idx]=cmd;
        buff_write_idx=(buff_write_idx+1)%CMD_BUFFER_DIM;
        cmdLock();
        num_comandos++;
        cmdUnlock();
        SemPost(&semLeitura);
}


/*semaforo functions*/

void initSemRead(){

    if(sem_init(&semLeitura,0,0)==-1){
        perror("Erro ao criar o semaforo de leitura");
        exit(EXIT_FAILURE);
    }

}

void initSemWrite(){

    if(sem_init(&semEscrita,0,CMD_BUFFER_DIM)==-1){
        perror("Erro ao criar o semaforo de leitura");
        exit(EXIT_FAILURE);
    }

}

void SemPost(sem_t* sem){

    if(sem_post(sem)==-1){
        perror("Erro ao fazer post no semaforo");
        exit(EXIT_FAILURE);
    }


}

void SemWait(sem_t* sem){

    if(sem_wait(sem)==-1){
        perror("Erro ao fazer post no semaforo");
        exit(EXIT_FAILURE);
    }


}

/*mutex functions*/

void initMutexRead(){
        pthread_mutex_init(&bufferReadMutex,NULL);
}

void ReadLock(){
        if(pthread_mutex_lock(&bufferReadMutex)!=0){
        fprintf(stderr,"Erro ao criar o Mutex");
        exit(EXIT_FAILURE);
    }
}

void ReadUnlock(){
        if(pthread_mutex_unlock(&bufferReadMutex)!=0){
        fprintf(stderr,"Error creating Mutex");
        exit(EXIT_FAILURE);
    }
}


/*simular functions*/

void initCond(){
        if (pthread_cond_init(&podeSimular,NULL)!=0){
                fprintf(stderr,"Erro ao criar a condição");
                exit(EXIT_FAILURE);
        }
}

void cmdInit(){
    pthread_mutex_init(&cmdsMutex,NULL);
    
}

void cmdLock(){
            if(pthread_mutex_lock(&cmdsMutex)!=0){
            fprintf(stderr,"Erro ao criar o Mutex");
            exit(EXIT_FAILURE);
        }
    }

void cmdUnlock(){
            if(pthread_mutex_unlock(&cmdsMutex)!=0){
            fprintf(stderr,"Error creating Mutex");
            exit(EXIT_FAILURE);
        }
    }


void waitPodeSimular(){
        if(pthread_cond_wait(&podeSimular,&cmdsMutex)!=0){
                fprintf(stderr,"Erro durante a espera da condição");
                exit(EXIT_FAILURE);
        }
}

void signalPodeSimular(){
            if(pthread_cond_signal(&podeSimular)!=0){
                    fprintf(stderr,"Erro durante a espera da condição");
                    exit(EXIT_FAILURE);
            }
    }

/*Pipe functions*/

void makePipe(){
  
  unlink(READ_PIPE);
  if(mkfifo(READ_PIPE,0777)<0){
    perror("não foi possivel criar o Pipe");
    exit(EXIT_SUCCESS);
  }
  
  if((readPipe=open(READ_PIPE,O_RDONLY))<0){
    perror("não foi possivel abrir o Pipe");
    exit(EXIT_SUCCESS);
  }
  
}

comando_t readCommand(){
  comando_t cmd;
  if(read(readPipe,&cmd,sizeof(comando_t))<0){
   perror("Erro ao ler o comando");
   cmd.operacao=-1; /*retorna uma operacao invalida*/
  }
   return cmd;
}

void writeResult(int res, char* pipe){
    int pipeTerminal;
    pipeTerminal=open(pipe,O_WRONLY);
    write(pipeTerminal,&res,sizeof(res));
    close(pipeTerminal);
}