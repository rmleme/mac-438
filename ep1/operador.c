// Autores: Leonardo Sunao Miyagi                Numero USP: 3116540
//          Rodrigo Mendes Leme                  Numero USP: 3151151
// Curso: computacao                             Data: 31/03/2002
// Professor: Reverbel
// Exercicio Programa 1
// Compilador usado: GCC-Linux
// Descricao: simula um atendente de call center, iniciando e terminando chama-
//            das telefonicas e registrando as estatisticas das chamadas, de
//            forma concorrente.

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include "ep1.h"

#define SHM_MODE (SHM_R | SHM_W)    // Leitura/escrita

int *shmptr_ongoing,              // Ligacoes sendo atendidas
    *shmptr_finished,             // Ligacoes ja encerradas
    *shmptr_soma_finished,        // Total de ligacoes encerradas
    *shmptr_in_op,                // Variavel de controle do Tie-Breaker
    *shmptr_in_sup,               // Variavel de controle do Tie-Breaker
    *shmptr_last,                 // Variavel de controle do Tie-Breaker
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


// Funcao: acessa_mem_compartilhada
// Descricao: acessa uma regiao de memoria compartilhada, possibilitando IPC.

void acessa_mem_compartilhada(void)
{

  if ((mykey = ftok("teste",'1')) == -1)
  {
    printf("Erro em ftok(): %s.\n",strerror(errno));
    exit(1);
  }
  if ((shmid = shmget(mykey,0,SHM_MODE)) < 0)
  {
    printf("Erro em shmget(): %s.\n",strerror(errno));
    exit(1);
  }
  if ((shmptr_ongoing = shmat(shmid,0,0)) == (void *) -1)
    printf ("Nao pode acessar memoria compartilhada.\n");
  shmptr_finished      = shmptr_ongoing + MAX_OPERADORES; 
  shmptr_soma_finished = shmptr_finished + MAX_OPERADORES;
  shmptr_in_op         = shmptr_soma_finished + 1;
  shmptr_in_sup        = shmptr_in_op + 1;
  shmptr_last          = shmptr_in_sup + 1;
}


// Funcao: cadastra_operador
// Saida: o indice do operador, se houver espaco para ele no sistema. Um erro,
//        caso contrario.
// Descricao: cadastra um operador no sistema, reservando para ele uma parte da
//            memoria compartilhada.

void cadastra_operador(int *indice_op)
{
  int i;

  // Algoritmo Tie Breaker para 2 processos
  while (TRUE)
  {
    *shmptr_last  = OPERADOR;
    *shmptr_in_op = TRUE;
    while (*shmptr_in_sup && *shmptr_last == OPERADOR) continue;
    // Entra na Regiao Critica
    for (i = 0; i < MAX_OPERADORES; i++)
      if (*(shmptr_ongoing + i) == LIVRE)
      {
        *(shmptr_ongoing + i)  = 0;
        *(shmptr_finished + i) = 0;
        *indice_op             = i;
        break;
      }
    // Sai da Regiao Critica
    *shmptr_in_op = FALSE;
    break;
  }

  if (i == MAX_OPERADORES)
    bye("numero maximo de operadores alcancado");
}


// Funcao: simula_atendimentos
// Entrada: o indice do operador.
// Descricao: le os comandos do operador, executando a acao apropriada para ca-
//            da um deles.

void simula_atendimentos (int indice_op)
{
  char cmd[256];

  while (TRUE)
  {
    scanf("%s",cmd);
    if (strcmp(cmd,"startcall") == 0)
    {
      // Algoritmo Tie Breaker para 2 processos
      while (TRUE)
      {
        *shmptr_last  = OPERADOR;
        *shmptr_in_op = TRUE;
        while (*shmptr_in_sup && *shmptr_last == OPERADOR) continue;
        // Entra na Regiao Critica
        (*(shmptr_ongoing + indice_op))++;
        // Sai da Regiao Critica
        *shmptr_in_op = FALSE;
        break;
      }
    }
    else if (strcmp(cmd,"endcall") == 0)
    {
      // Algoritmo Tie Breaker para 2 processos
      while (TRUE)
      {
        *shmptr_last  = OPERADOR;
        *shmptr_in_op = TRUE;
        while (*shmptr_in_sup && *shmptr_last == OPERADOR) continue;
        // Entra na Regiao Critica
        if (*(shmptr_ongoing + indice_op) > 0)
	{
          (*(shmptr_ongoing + indice_op))--;
          (*(shmptr_finished + indice_op))++;
          (*shmptr_soma_finished)++;
        }
        else
          printf("\nNenhuma chamada ativa no momento.\n\n");
        // Sai da Regiao Critica
        *shmptr_in_op = FALSE;
        break;
      }
    }
    else if (strcmp(cmd,"report") == 0)
    {
      // Algoritmo Tie Breaker para 2 processos
      while (TRUE)
      {
        *shmptr_last  = OPERADOR;
        *shmptr_in_op = TRUE;
        while (*shmptr_in_sup && *shmptr_last == OPERADOR) continue;
        // Entra na Regiao Critica
        printf ("\nChamadas ativas: %d     Terminadas: %d\n\n",
                *(shmptr_ongoing + indice_op),*(shmptr_finished + indice_op));
        // Sai da Regiao Critica
        *shmptr_in_op = FALSE;
        break;
      }
    }
    else if (strcmp(cmd,"exit") == 0) 
    {
      // Algoritmo Tie Breaker para 2 processos
      while (TRUE)
      {
        *shmptr_last  = OPERADOR;
        *shmptr_in_op = TRUE;
        while (*shmptr_in_sup && *shmptr_last == OPERADOR) continue;
        // Entra na Regiao Critica
        *(shmptr_ongoing + indice_op)  = LIVRE;
        *(shmptr_finished + indice_op) = LIVRE;
        // Sai da Regiao Critica
        *shmptr_in_op = FALSE;
        break;
      }
      printf("Operador finalizado.");
      exit(1);
    }
    else
      printf("Comando invalido.\n");
  }
}


// Funcao: main
// Descricao: processa os argumentos recebidos na linha de comando e inicializa
//            a execucao do programa.

int main(int argc, char *argv[]) 
{
  pid_t supervisor_pid;
  int indice_op;

  if (argc != 2)
    bye("falta o pid do supervidor");
  supervisor_pid = atoi(argv[1]);
  if (supervisor_pid == 0)
    bye("pid invalido");

  acessa_mem_compartilhada();
  cadastra_operador(&indice_op);
  simula_atendimentos(indice_op);  
  return 0;
}
