#include "contas.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define atrasar() sleep(ATRASO)
#define novoSaldo(A) lerSaldo(A)*(1+TAXAJURO)-CUSTOMANUTENCAO		      		     
#define creditoSimula(A) novoSaldo(A)-lerSaldo(A)
		     
int contasSaldos[NUM_CONTAS];

extern int signal


int contaExiste(int idConta) {
  return (idConta > 0 && idConta <= NUM_CONTAS);
}


/*
 Poe todas contas (que existem) com saldo zero.
 */
void inicializarContas() {
  int i;
  for (i=0; i<NUM_CONTAS; i++)
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
  if (contasSaldos[idConta - 1] < valor)
    return -1;
  atrasar();
  contasSaldos[idConta - 1] -= valor;
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
  contasSaldos[idConta - 1] += valor;
  return 0;
}


/**
 * Recebe id da conta.
 * Le saldo respectivo.
 */
int lerSaldo(int idConta) {
  atrasar();
  if (!contaExiste(idConta))
    return -1;
  return contasSaldos[idConta - 1];
}


void simular(int numAnos) {
	int ano,idConta;
			for (ano=0; ano<=numAnos;ano++){
				printf("SIMULACAO: Ano %d\n",(ano-1));
				printf("=================\n");
				for (idConta=1; idConta <= NUM_CONTAS;idConta++){

					printf("Conta %d, Saldo %d\n",idConta,lerSaldo(idConta);
					
					if (novoSaldo(idconta)>0)
						creditar(creditoSimula(idConta))
					else
						debitar(lerSaldo(idConta))
				}
				printf("\n");	
			}
}