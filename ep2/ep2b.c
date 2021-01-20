// Autores: Leonardo Sunao Miyagi                Numero USP: 3116540
//          Rodrigo Mendes Leme                  Numero USP: 3151151
// Curso: computacao                             Data: 11/05/2002
// Professor: Reverbel
// Exercicio Programa 2b
// Compilador usado: GCC-Linux
// Descricao: simula a travessia de uma ponte por carros vindos do sul e do
//            norte, de maneira concorrente, de forma imparcial.

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct CARRO * ap_carro;
typedef struct CARRO
        {
          int indice_carro;
          ap_carro prox;
        } t_carro;

int nn = 0,             // Numero de carros atravessando para o norte
    ns = 0,             // Numero de carros atravessando para o sul
    dn = 0,             // Numero de carros esperando travessia para o norte
    ds = 0,             // Numero de carros esperando travessia para o sul
    *tempo_travessias,          // Tempo que cada carro levara na travessia
    *tempo_permanencias,        // Tempo que cada carro tera esperando num lado
    qtas_travessias;    // Numero de travessias que os carros farao na ponte
double tempos_norte = 0,        // Soma dos tempos dos carros que vem do sul
       tempos_sul   = 0;        // Soma dos tempos dos carros que vem do norte
sem_t e,                // Controla entrada na ponte
      norte,            // Controla fluxo de carros para o norte
      sul,              // Controla fluxo de carros para o sul
      saida,            // Quem pode sair da ponte
      *vez;             // Sinaliza a vez do carro sair da ponte
t_carro *pri_carro = NULL,       // Primeiro e ultimo carros da fila dos carros
        *ult_carro = NULL;       // que esperam para entrar na ponte
struct timeval *t_real_ini,      // Usado na contagem de tempo da simulacao
               *t_real_fim;      // Usado na contagem de tempo da simulacao
struct timezone tz;              // Usado na contagem de tempo da simulacao


// Funcao: count
// Entrada: c: quando c == 1, comeca a contagem. Caso contrario, termina.
//          i: indice do carro cujo tempo sera contado.
// Saida: o intervalo de tempo contado.
// Descricao: conta o tempo entre dois instantes.

double count(int c, int i)
{
  if (c == 1)
  {
    gettimeofday(&t_real_ini[i],&tz);
    return -1;
  }
  else
  {
    gettimeofday(&t_real_fim[i],&tz);
    return t_real_fim[i].tv_sec - t_real_ini[i].tv_sec;
  }
}


// Funcao: enfileira
// Entrada: o indice do carro que sera enfileirado.
// Descricao: insere um carro na fila de carros que estao atravessando a ponte.

void enfileira(int i)
{
  ap_carro novo_carro;

  novo_carro = (ap_carro) malloc(sizeof(t_carro));
  novo_carro->indice_carro = i;
  novo_carro->prox = NULL;
  if (pri_carro == NULL)       // A lista esta vazia
    pri_carro = novo_carro;
  else
    ult_carro->prox = novo_carro;
  ult_carro = novo_carro;
}


// Funcao: desenfileira
// Saida: o indice do carro desenfileirado.
// Descricao: remove um carro da fila de carros que estao atravessando a ponte.


int desenfileira(void)
{
  int indice_carro;

  if ((pri_carro == NULL) && (ult_carro == NULL))     // A lista esta vazia
    return -1;
  indice_carro = pri_carro->indice_carro;
  pri_carro = pri_carro->prox;
  if (pri_carro == NULL)         // A lista so tinha um carro
    ult_carro = NULL;
  return indice_carro;
}


// Funcao: funcao_controladora
// Descricao: funcao concorrente que controla a saida de carros da ponte.

void funcao_controladora(void)
{
  int i;

  while (1)
  {
    sem_wait(&saida);
    i = desenfileira();
    sem_post(&vez[i]);
  }
}

// Funcao: passeia_pelo_norte
// Entrada: tempo de permanencia no lado norte da ponte.
// Descricao: faz um carro esperar um dado intervalo de tempo antes de tentar
//            atravessar a ponte.

void passeia_pelo_norte(int tempo_permanece)
{
  sleep(tempo_permanece);
}

// Funcao: passeia_pelo_sul
// Entrada: tempo de permanencia no lado sul da ponte.
// Descricao: faz um carro esperar um dado intervalo de tempo antes de tentar
//            atravessar a ponte.

void passeia_pelo_sul(int tempo_permanece)
{
  sleep(tempo_permanece);
}


// Funcao: atravessa_ponte
// Entrada: i: indice do carro que esta atravessando a ponte.
//          sentido: para qual sentido o carro esta se dirigindo.
// Descricao: executa a acao propriamente dita de travessia da ponte, ajustando
//            a velocidade do carro se necessario.

void atravessa_ponte(int i, char *sentido)
{
  ap_carro atu;
  int max_travessia = 0,
      tempo_trav_real;

  for (atu = pri_carro; (atu != NULL) && (atu->indice_carro != i);
       atu = atu->prox)
    if (tempo_travessias[atu->indice_carro] > max_travessia)
      max_travessia = tempo_travessias[atu->indice_carro];
  if (tempo_travessias[i] < max_travessia)     // Tem carro mais lento a frente
    tempo_trav_real = max_travessia;
  else
    tempo_trav_real = tempo_travessias[i];
  printf("Carro %d esta atravessando a ponte para o %s.\n",i,sentido);
  sleep(tempo_trav_real);
}

// Funcao: signal_norte
// Descricao: acorda um carro que esteja esperando para atravessar a ponte,
//            dando preferencia a um carro que esteja no lado sul.

void signal_norte(void)
{
  if ((ds > 0) && (nn == 0))
  {
    ds--;
    sem_post(&sul);
  }
  else
    sem_post(&e);
}


// Funcao: signal_sul
// Descricao: acorda um carro que esteja esperando para atravessar a ponte,
//            dando preferencia a um carro que esteja no lado norte.

void signal_sul(void)
{
  if ((dn > 0) && (ns == 0))
  {
    dn--;
    sem_post(&norte);
  }
  else
    sem_post(&e);
}

// Funcao: espera_passagem_aberta_para_norte
// Entrada: i: o indice do carro.
// Descricao: verifica se existe algum carro vindo no sentido contrario. Se
//            houver, coloca o carro para dormir.

void espera_passagem_aberta_para_norte(int i)
{
  count(1,i);
  sem_wait(&e);
  if ((ns > 0) || (ds > 0))          // Tem carro na contra-mao
  {
    dn++;
    sem_post(&e);
    sem_wait(&norte);          // Espera ate poder atravessar
  }
}


// Funcao: atravessa_ponte_para_norte
// Entrada: i: o indice do carro.
// Descricao: coloca o carro na fila de entrada da ponte e executa a travessia.

void atravessa_ponte_para_norte(int i)
{
  tempos_norte += count(0,i);
  nn++;
  enfileira(i);
  if (nn == 1)
    sem_post(&saida);
  signal_norte();
  atravessa_ponte(i,"norte");
  sem_wait(&vez[i]);
  sem_wait(&e);
  nn--;
  if (nn > 0)
    sem_post(&saida);
  signal_norte();
}


// Funcao: espera_passagem_aberta_para_sul
// Entrada: i: o indice do carro.
// Descricao: verifica se existe algum carro vindo no sentido contrario. Se
//            houver, coloca o carro para dormir.

void espera_passagem_aberta_para_sul(int i)
{
  count(1,i);
  sem_wait(&e);
  if ((nn > 0) || (dn > 0))          // Tem carro na contra-mao
  {
    ds++;
    sem_post(&e);
    sem_wait(&sul);          // Espera ate poder atravessar
  }
}


// Funcao: atravessa_ponte_para_sul
// Entrada: i: o indice do carro.
// Descricao: coloca o carro na fila de entrada da ponte e executa a travessia.

void atravessa_ponte_para_sul(int i)
{
  tempos_sul += count(0,i);
  ns++;
  enfileira(i);
  if (ns == 1)
    sem_post(&saida);
  signal_sul();
  atravessa_ponte(i,"sul");
  sem_wait(&vez[i]);
  sem_wait(&e);
  ns--;
  if (ns > 0)
    sem_post(&saida);
  signal_sul();
}


// Funcao: funcao_thread_sul
// Entrada: indice_carro: o indice do carro relacionado a thread.
// Descricao: funcao concorrente que simula o percurso do carro pela ponte e
//            fora dela, comecando pelo lado sul.

void funcao_thread_sul(void *indice_carro)
{
  int tempo_permanece = tempo_permanencias[(int) indice_carro],
      trav_feitas     = 0;

  while (trav_feitas < qtas_travessias)
  {
    passeia_pelo_sul(tempo_permanece);
    espera_passagem_aberta_para_norte((int) indice_carro);
    atravessa_ponte_para_norte((int) indice_carro);
    trav_feitas++;
    if (trav_feitas < qtas_travessias)
    {
      passeia_pelo_norte(tempo_permanece);
      espera_passagem_aberta_para_sul((int) indice_carro);
      atravessa_ponte_para_sul((int) indice_carro);
      trav_feitas++;
    }
  }
  if ((ds > 0) && (nn == 0))
  {
    ds--;
    sem_post(&sul);
  }
  else
    if ((dn > 0) && (ns == 0))
    {
      dn--;
      sem_post(&norte);
    }
}


// Funcao: funcao_thread_norte
// Entrada: indice_carro: o indice do carro relacionado a thread.
// Descricao: funcao concorrente que simula o percurso do carro pela ponte e
//            fora dela, comecando pelo lado norte.

void funcao_thread_norte(void *indice_carro)
{
  int tempo_permanece = tempo_permanencias[(int) indice_carro],
      trav_feitas     = 0;

  while (trav_feitas < qtas_travessias)
  {
    passeia_pelo_norte(tempo_permanece);
    espera_passagem_aberta_para_sul((int) indice_carro);
    atravessa_ponte_para_sul((int) indice_carro);
    trav_feitas++;
    if (trav_feitas < qtas_travessias)
    {
      passeia_pelo_sul(tempo_permanece);
      espera_passagem_aberta_para_norte((int) indice_carro);
      atravessa_ponte_para_norte((int) indice_carro);
      trav_feitas++;
    }
  }
  if ((ds > 0) && (nn == 0))
  {
    ds--;
    sem_post(&sul);
  }
  else
    if ((dn > 0) && (ns == 0))
    {
      dn--;
      sem_post(&norte);
    }
}


// Funcao: main
// Descricao: processa os argumentos recebidos na linha de comando e inicializa
//            a execucao do programa.  if ((ds > 0) && (nn == 0))

int main(int argc, char *argv[])
{
  int i,
      qtos_carros_inic,        // Numero de carros de cada lado da ponte
      tempo_min_trav,          // Valor minimo do tempo de travessia da ponte
      tempo_max_trav,          // Valor maximo do tempo de travessia da ponte
      tempo_min_esp,           // Valor minimo do tempo de espera no mesmo lado
      tempo_max_esp;           // Valor maximo do tempo de espera no mesmo lado
  double media_esp_sul,        // Tempo medio para entrar na ponte pelo norte
         media_esp_norte,      // Tempo medio para entrar na ponte pelo sul
         num_aleat,            // Usados na criacao dos tempos
         delta_tempo_trav,     // de travessia e de permanencia
         delta_tempo_esp;      // dos carros
  pthread_t *carros,           // Threads que simulam os carros
            controlador;       // Thread que controla quem sai da ponte

  if (argc != 7)
  {
    printf("Erro: numero de argumentos invalidos.\n");
    return -1;
  }
  qtos_carros_inic = atoi(argv[1]);
  tempo_min_trav   = atoi(argv[2]);
  tempo_max_trav   = atoi(argv[3]);
  tempo_min_esp    = atoi(argv[4]);
  tempo_max_esp    = atoi(argv[5]);
  qtas_travessias  = atoi(argv[6]);

  // Inicializacao dos semaforos
  sem_init(&e,0,1);
  sem_init(&norte,0,0);
  sem_init(&sul,0,0);
  sem_init(&saida,0,0);
  vez = (sem_t *) malloc(2 * qtos_carros_inic * sizeof(sem_t));
  for (i = 0; i < 2 * qtos_carros_inic; i++)
    sem_init(&vez[i],0,0);

  // Inicializacao das estruturas de contagem de tempo
  t_real_ini = (struct timeval *) malloc(2 * qtos_carros_inic *
               sizeof(struct timeval));
  t_real_fim = (struct timeval *) malloc(2 * qtos_carros_inic *
               sizeof(struct timeval));

  // Cada carro tera um tempo de travessia e um tempo de permanencia
  tempo_travessias   = (int *) malloc(2 * qtos_carros_inic * sizeof(int));
  tempo_permanencias = (int *) malloc(2 * qtos_carros_inic * sizeof(int));
  delta_tempo_trav   = (double) tempo_max_trav - tempo_min_trav;
  delta_tempo_esp    = (double) tempo_max_esp - tempo_min_esp;
  for (i = 0; i < 2 * qtos_carros_inic; i++)
  {
    num_aleat = (double) rand() / RAND_MAX;
    tempo_travessias[i] = (int) ((double) (num_aleat * delta_tempo_trav) +
                          tempo_min_trav);
    num_aleat = (double) rand() / RAND_MAX;
    tempo_permanencias[i] = (int) ((double) (num_aleat * delta_tempo_esp) +
                            tempo_min_esp);
  }

  // Cria o processo controlador
  pthread_create(&controlador,NULL,(void *) &funcao_controladora,(void *) i);

  // Cria os carros
  carros = (pthread_t *) malloc(2 * qtos_carros_inic * sizeof(pthread_t));
  for (i = 0; i < 2 * qtos_carros_inic; i++)
    if (i % 2 == 0)
      pthread_create(carros + i,NULL,(void *) &funcao_thread_sul,(void *) i);
    else
      pthread_create(carros + i,NULL,(void *) &funcao_thread_norte,(void *) i);

  // Obriga o programa principal a aguardar o termino das outras threads
  for (i = 0; i < 2 * qtos_carros_inic; i++)
    pthread_join(carros[i],NULL);

  media_esp_norte = (double) tempos_norte / (2 * qtos_carros_inic *
                    qtas_travessias);
  media_esp_sul = (double) tempos_sul / (2 * qtos_carros_inic *
                  qtas_travessias);
  printf("\nTempo medio de espera para atravessar do sul para o norte: %.2g\n",
         media_esp_norte);
  printf("Tempo medio de espera para atravessar do norte para o sul: %.2g\n",
         media_esp_sul);

  return 0;
}
