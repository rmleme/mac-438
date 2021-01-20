// Autores: Leonardo Sunao Miyagi                Numero USP: 3116540
//          Rodrigo Mendes Leme                  Numero USP: 3151151
// Curso: computacao                             Data: 31/03/2002
// Professor: Reverbel
// Exercicio Programa 1
// Compilador usado: GCC-Linux
// Descricao: simula um supervisor de call center, coletando informacoes a res-
//            peito das chamadas atendidas pelos supervisores e imprimindo es-
//            sas informacoes, periodicamente, na tela (de forma concorrente).

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include "ep1.h"

#define SHM_MODE (SHM_R | SHM_W | IPC_CREAT)    // Leitura/escrita/criacao

int *shmptr_ongoing,                // Ligacoes sendo atendidas
    *shmptr_finished,               // Ligacoes ja encerradas
    *shmptr_soma_finished,          // Total de ligacoes encerradas
    *shmptr_in_op,                  // Variavel de controle do Tie-Breaker
    *shmptr_in_sup,                 // Variavel de controle do Tie-Breaker
    *shmptr_last,                   // Variavel de controle do Tie-Breaker
    shmid;
key_t mykey;


// Funcao: bye
// Entrada: um texto explicativo.
// Descricao: escreve uma mensagem de erro e sai do programa.

static void bye(const char* string) 
{
  printf("Erro: %s.\n",string);
  exit(1);
}


// Funcao: cria_mem_compartilhada
// Descricao: cria uma regiao de memoria compartilhada, possibilitando IPC.

void cria_mem_compartilhada(void)
{
  int tamanho = 2 * MAX_OPERADORES + 4;

  if ((mykey = ftok("teste",'1')) == -1)
  {
    printf("Erro em ftok(): %s.\n",strerror(errno));
    exit(1);
  }
  if ((shmid = shmget(mykey,tamanho,SHM_MODE)) < 0)
  {
    printf("Erro em shmget(): %s.\n",strerror(errno));
    exit(1);
  }
  if ((shmptr_ongoing = shmat(shmid,0,0)) == (void *) -1)
    printf("Nao pode acessar memoria compartilhada.\n");
  shmptr_finished      = shmptr_ongoing + MAX_OPERADORES;
  shmptr_soma_finished = shmptr_finished + MAX_OPERADORES;
  shmptr_in_op         = shmptr_soma_finished + 1;
  shmptr_in_sup        = shmptr_in_op + 1;
  shmptr_last          = shmptr_in_sup + 1;
}


// Funcao: inicializa_vetores
// Descricao: inicializa os vetores de ligacoes atendidas e terminadas: LIVRE
//            indica que a posicao i esta livre; caso contrario, esta sendo
//            ocupada por um operador.

void inicializa_vetores(void)
{
  int i;

  for (i = 0; i < MAX_OPERADORES; i++)
  {
    *(shmptr_ongoing + i)  = LIVRE;
    *(shmptr_finished + i) = LIVRE;
  }
  *shmptr_soma_finished = 0;
}


// Funcao: simula_atendimentos
// Entrada: interval: duracao em segundos do intervalo de tempo entre a exibi-
//          cao de informacoes.
// Descricao: imprime informacoes a respeito das chamadas atendidas pelos ope-
//            radores.

void simula_atendimentos(int interval)
{
  int i,
      soma_ongoing,
      soma_finished;

  printf("Supervisor ativado. Aperte [CTRL-C] para finalizar.\n\n");
  while (TRUE)
  {
    sleep(interval);
    soma_ongoing = soma_finished = 0;
    printf("Operadores ativos no momento:\n\n");

    // Algoritmo Tie Breaker para 2 processos
    while (TRUE)
    {
      *shmptr_last   = SUPERVISOR;
      *shmptr_in_sup = TRUE;
      while (*shmptr_in_op && *shmptr_last == SUPERVISOR) continue;
      // Entra na Regiao Critica
      for (i = 0; i < MAX_OPERADORES; i++)
        if (*(shmptr_ongoing + i) != LIVRE)
        {
          soma_ongoing  += *(shmptr_ongoing + i);
          printf("Operador %d:\nChamadas ativas: %d     Terminadas: %d\n\n",i,
                 *(shmptr_ongoing + i),*(shmptr_finished + i));
        }
      soma_finished = *shmptr_soma_finished;
      // Sai da Regiao Critica
      *shmptr_in_sup = FALSE;
      break;
    }

    printf("-------------------------------------------------\n");
    printf("Total: Ativas: %d     Terminadas: %d\n",soma_ongoing,
           soma_finished);
    printf("=================================================\n\n\n");
  }
}


// Funcao: main
// Descricao: processa os argumentos recebidos na linha de comando e inicializa
//            a execucao do programa.

int main(int argc, char *argv[]) 
{
  int interval;

  printf("Meu pid eh %d\n\n",getpid());

  if (argc != 2)
    bye("numero errado de argumentos");
  interval = atoi(argv[1]);
  if (interval == 0)
    bye("intervalo invalido");

  cria_mem_compartilhada();
  inicializa_vetores();
  simula_atendimentos(interval);
  if (shmctl(shmid,IPC_RMID,0) < 0 )
    printf("Nao pode remover: erro em shmctl().\n");
  return 0;
}
