#include "contas.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define atrasar() sleep(ATRASO)
		     
int contasSaldos[NUM_CONTAS];


int contaExiste(int idConta) {
  return (idConta > 0 && idConta <= NUM_CONTAS);
}

void inicializarContas() {
  int i;
  for (i=0; i<NUM_CONTAS; i++)
    contasSaldos[i] = 0;
}

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

int creditar(int idConta, int valor) {
  atrasar();
  if (!contaExiste(idConta))
    return -1;
  contasSaldos[idConta - 1] += valor;
  return 0;
}


/* -= getSaldo*/
int lerSaldo(int idConta) {
  atrasar();
  if (!contaExiste(idConta))
    return -1;
  return contasSaldos[idConta - 1];
}

int maxSaldo(int n){
    if (n>0)
        return n;
    else
        return 0;
}



/*
 TAXAJURO
CUSTOMANUTENCAO 
 */

int newSaldo(int s1){
    
int s2;
s2 = s1*(1+TAXAJURO)-CUSTOMANUTENCAO;
res = maxSaldo(s2);

return res;
}

void simular(int numAnos) {
int i,s1,s2;

        for (i=0; i <= NUM_CONTAS;i++){
            
            int s1 = lerSaldo(i);
            int s2 = newSaldo(s1);
            
            
        }
      
}
