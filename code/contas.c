


#include "contas.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define atrasar() sleep(ATRASO)

int contasSaldos[NUM_CONTAS];
pthread_mutex_t MutexConta[NUM_CONTAS];
int sinalRecebido;

void lockConta(int id);
void unlockConta(int id);

int contaExiste(int idConta) {
    return (idConta > 0 && idConta <= NUM_CONTAS);
}

/*
 Poe todas contas (que existem) com saldo zero.
 */
void inicializarContas() {
  int i;
  for (i=0; i<NUM_CONTAS; i++)
    pthread_mutex_init(&MutexConta[i],NULL);
    contasSaldos[i] = 0;
}



/**
 * Recebe id da conta e valor a depositar.
 * Tira esse valor em dinheiro na conta desse id dado.
 */
int debitar(int idConta, int valor) {
  atrasar();
  

  
  if (!contaExiste(idConta))
    return -1;
    
  lockConta(idConta);
  if (contasSaldos[idConta - 1] < valor){
    unlockConta(idConta);
	return -1;
  }

  atrasar();
  contasSaldos[idConta - 1] -= valor;
  unlockConta(idConta);
  
  return 0;
}



/**
 * Recebe id da conta e valor a depositar.
 * Poe esse valor em dinheiro na conta desse id dado.
 */
int creditar(int idConta, int valor) {
  atrasar();
  

  
  if (!contaExiste(idConta))
    return -1;
    
  lockConta(idConta);
  contasSaldos[idConta - 1] += valor;
  unlockConta(idConta);
  
return 0;
}

/**
 * Recebe id da conta origem, valor a transferir e valor a transferir.
 * Tira esse valor em dinheiro na conta origem.
 * Poe esse valor em dinheiro na conta destino.
 */
int transferir(int idConta, int valor, int idContaDestino) {
    int idConta1 , idConta2; 
    idConta1 = idConta;
    idConta2 = idContaDestino;
    atrasar();
     
     
if (!contaExiste(idConta) || !contaExiste(idConta2))
    return -1;
    
  
  /*Correcao de ordem*/
  /*Caso de idContaDestino < idConta (de origem)*/
  if (idContaDestino < idConta){
      idConta1 = idContaDestino;
      idConta2 = idConta;
  }
  
  /*por ordem*/
  lockConta(idConta1);
  lockConta(idConta2);
  
  /*Nao pode mandar dinheiro se nao tiver suficiente*/
  if ((idConta == idContaDestino) || (contasSaldos[idConta - 1] < valor)){
    /*por ordem*/
    unlockConta(idConta1);
    unlockConta(idConta2);
    return -1;
  }

  
  contasSaldos[idConta - 1] -= valor;
  contasSaldos[idContaDestino - 1] += valor;
  
  /*por ordem*/
  unlockConta(idConta1);
  unlockConta(idConta2);
  
  return 0;
}


/**
 * Recebe id da conta.
 * Le saldo respectivo.
 */
int lerSaldo(int idConta) {
  int res;
  
  atrasar();

  
  if (!contaExiste(idConta))
    return -1;

  lockConta(idConta);
   
  res = contasSaldos[idConta - 1];
  unlockConta(idConta);
  
  return res;
}


void simular(int numAnos) {
	int ano,idConta, saldo;
        for (ano=0; ano<=numAnos;ano++){
            printf("SIMULACAO: Ano %d\n",(ano));
            printf("=================\n");
            for (idConta=1; idConta <= NUM_CONTAS;idConta++){
                saldo = lerSaldo(idConta);
                printf("Conta %d, Saldo %d\n",idConta,saldo);
                creditar(idConta, saldo * TAXAJURO);
                saldo = lerSaldo(idConta);
                debitar(idConta, (CUSTOMANUTENCAO > saldo) ? saldo : CUSTOMANUTENCAO);
            }
            printf("\n");	
            if (sinalRecebido!=0){
                printf("Simulacao Terminada por Signal\n");
                exit(EXIT_SUCCESS);
            }
        }
}



void unlockConta(int id){
	if(pthread_mutex_unlock(&MutexConta[id-1])!=0){
		fprintf(stderr,"Erro ao desbloquear a conta");
		exit(EXIT_FAILURE);
		
	}
		
}


void lockConta(int id){
	if(pthread_mutex_lock(&MutexConta[id-1])!=0){
		fprintf(stderr,"Erro ao bloquear a conta");
		exit(EXIT_FAILURE);
		
	}
		
}
