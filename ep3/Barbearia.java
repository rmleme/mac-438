// Classe: Barbearia.java
// Autores: Leonardo Sunao Miyagi                  Numero USP: 3116540
//          Rodrigo Mendes Leme                    Numero USP: 3151151
// Exercicio Programa 3                            Data: 29/06/2002
// Descricao: implementacao de um pseudo-monitor que encapsula todas as
//            chamadas as variaveis compartilhadas entre threads.

public final class Barbearia
{
  private BarbeiroDisp barbeiro_disp;        // Ha barbeiros disponiveis?
  private CadeiraOcup cadeira_ocup;          // A cadeira esta sendo ocupada?
  private PortaAberta porta_aberta;          // A porta de saida esta aberta?
  private ClienteSaiu cliente_saiu;          // O cliente ja saiu da barbearia?
  private Representacao representacao;       // Dados compartilhados
  private Barbeiro[] barbeiros;              // Threads dos barbeiros
  private Cliente[] clientes;                // Threads dos clientes
  private int num_cadeiras;                  // Numero de cadeiras de espera

  public Barbearia(int num_cad, int num_cli, int num_barb)
  {
    int i;

    num_cadeiras = num_cad;

    representacao                = new Representacao();
    representacao.barbeiros      = new int[num_barb];
    representacao.barbeiros_disp = 0;
    representacao.cadeira        = 0;
    representacao.porta_aberta   = 0;
    for (i = 0; i < num_barb; i++)
      representacao.barbeiros[i] = 0;

    barbeiro_disp = new BarbeiroDisp(this,representacao,num_cad);
    cadeira_ocup  = new CadeiraOcup(this,representacao);
    porta_aberta  = new PortaAberta(this,representacao);
    cliente_saiu  = new ClienteSaiu(this,representacao);

    barbeiros = new Barbeiro[num_barb];
    for (i = 0; i < num_barb; i++)
      barbeiros[i] = new Barbeiro(this,i);

    clientes = new Cliente[num_cli];
    for (i = 0; i < num_cli; i++)
      clientes[i] = new Cliente(this,i);
  }

  // Metodo: abreBarbearia
  // Descricao: inicializa as threads dos barbeiros e dos clientes.

  public void abreBarbearia()
  {
    int i;

    for (i = 0; i < barbeiros.length; i++)
      barbeiros[i].start();
    for (i = 0; i < clientes.length; i++)
      clientes[i].start();
  }

  // Metodo: cortaCabelo
  // Entrada: idCliente: identificador do cliente que quer cortar o cabelo.
  // Saida: true se o cliente conseguiu cortar o cabelo; false caso contrario
  //        ou se a thread foi interrompida anormalmente.
  // Descricao: o cliente entra na barbearia e ve se e possivel cortar o
  //            cabelo.
  public boolean cortaCabelo(int idCliente)
  {
    org.omg.CORBA.IntHolder idBarbeiro = new org.omg.CORBA.IntHolder();

    try {
      if (barbeiro_disp.cortaCabelo(idBarbeiro))
      {
        System.out.println("O barbeiro " + idBarbeiro.value + " esta cortando o cabelo do cliente " + idCliente + ".......");
        porta_aberta.cortaCabelo();
        return true;
      }
      else
        return false;
    }
    catch(InterruptedException ie) {
      System.out.println("Erro: interrupcao anormal do programa.");
      System.out.println();
      return false;
    }
  }

  // Metodo: proximoCliente
  // Entrada: idBarbeiro: identificador do barbeiro.
  // Descricao: o barbeiro chama o proximo cliente da fila.

  public void proximoCliente(int idBarbeiro)
  {
    try {
      cadeira_ocup.proximoCliente(idBarbeiro);
    }
    catch(InterruptedException ie) {
      System.out.println("Erro: interrupcao anormal do programa.");
      return;
    }
  }

  // Metodo: corteTerminado
  // Descricao: o barbeiro libera o cliente, apos o termino do corte.

  public void corteTerminado()
  {
    try {
      cliente_saiu.corteTerminado();
    }
    catch(InterruptedException ie) {
      System.out.println("Erro: interrupcao anormal do programa.");
      return;
    }
  }

  // Metodo: signal
  // Entrada: bd: monitor que indica a disponibilidade de barbeiros.
  // Descricao: acorda um cliente que esteja esperando para sentar na cadeira
  //            do barbeiro.

  public void signal(BarbeiroDisp bd)
  {
    cadeira_ocup.signal();
  }

  // Metodo: signal
  // Entrada: co: monitor que indica a ocupacao ou nao de uma cadeira.
  //          idBarbeiro: identificador do barbeiro.
  // Descricao: acorda um barbeiro que esteja esperando para cortar o cabelo de
  //            um cliente.

  public void signal(CadeiraOcup co, int idBarbeiro)
  {
    barbeiro_disp.signal(idBarbeiro);
  }

  // Metodo: signal
  // Entrada: pa: monitor que indica se a porta de saida esta aberta ou nao.
  // Descricao: acorda um cliente que tenha terminado o corte para que ele
  //            possa sair da barbearia.

  public void signal(PortaAberta pa)
  {
    cliente_saiu.signal();
  }

  // Metodo: signal
  // Entrada: cs: monitor que indica se o cliente ja saiu ou nao.
  // Descricao: acorda um barbeiro para que ele possa abrir a porta de saida.

  public void signal(ClienteSaiu cs)
  {
    porta_aberta.signal();
  }
}
