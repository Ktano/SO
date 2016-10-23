#include "contas.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define atrasar() sleep(ATRASO)
#define valorCreditar(A) (lerSaldo(A)*(TAXAJURO)-CUSTOMANUTENCAO)

int contasSaldos[NUM_CONTAS];
pthread_mutex_t MutexConta[NUM_CONTAS]; /**/
int sinalRecebido;



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



/**CUSTOMANUTENCAOerSaldo(idConta)<=(-
 * Recebe id da conta e valor a depositar.
 * Tira esse valor em dinheiro na conta desse id dado.
 */
int debitar(int idConta, int valor) {
  atrasar();
  
  pthread_mutex_lock(&MutexConta[idConta]);
  
  if (!contaExiste(idConta))
    return -1;
  if (contasSaldos[idConta - 1] < valor)
    return -1;
  atrasar();
  contasSaldos[idConta - 1] -= valor;
  
  pthread_mutex_unlock(&MutexConta[idConta]);
  
  return 0;
}



/**
 * Recebe id da conta e valor a depositar.
 * Poe esse valor em dinheiro na conta desse id dado.
 */
int creditar(int idConta, int valor) {
  atrasar();
  
  pthread_mutex_lock(&MutexConta[idConta]);
  
  if (!contaExiste(idConta))
    return -1;
  contasSaldos[idConta - 1] += valor;
  
  pthread_mutex_unlock(&MutexConta[idConta]);
  
return 0;
}


/**
 * Recebe id da conta.
 * Le saldo respectivo.
 */
int lerSaldo(int idConta) {
  atrasar();

   pthread_mutex_lock(&MutexConta[idConta]);
  
  if (!contaExiste(idConta))
    return -1;

 pthread_mutex_unlock(&MutexConta[idConta]);
  
  return contasSaldos[idConta - 1]; /*duvida*/
}


void simular(int numAnos) {
	int ano,idConta;
		for (ano=0; ano<=numAnos;ano++){
			printf("SIMULACAO: Ano %d\n",(ano));
			printf("=================\n");
			for (idConta=1; idConta <= NUM_CONTAS;idConta++){
				
                                
                                int vCreditar;
				printf("Conta %d, Saldo %d\n",idConta,lerSaldo(idConta));
				vCreditar = valorCreditar(idConta);
				if (vCreditar>0)
					creditar(idConta,vCreditar); /*credita o valor na conta para obter o novo saldo*/
				else					    
					debitar(idConta,  ( ( lerSaldo(idConta)>=abs(vCreditar) ) ?  abs(vCreditar):lerSaldo(idConta)));
			}
			printf("\n");	
			if (sinalRecebido!=0){
				printf("Simulacao Terminada por Signal\n");
				exit(EXIT_SUCCESS);
			}
		}
}
