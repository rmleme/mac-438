// Classe: BarbeiroDisp.java
// Autores: Leonardo Sunao Miyagi                  Numero USP: 3116540
//          Rodrigo Mendes Leme                    Numero USP: 3151151
// Exercicio Programa 3                            Data: 29/06/2002
// Descricao: helper utilizado pela classe Barbearia para fazer acessos
//            sincronizados a condicao "barbeiros disponiveis".

public class BarbeiroDisp
{
  private Barbearia barbearia;
  private Representacao representacao;
  private int esperando;
  private int num_cadeiras;

  public BarbeiroDisp(Barbearia b, Representacao r, int num_cad)
  {
    barbearia     = b;
    representacao = r;
    esperando     = 0;
    num_cadeiras  = num_cad;
  }

  // Metodo: signal
  // Entrada: idBarbeiro: identificador de um barbeiro.
  // Descricao: caso haja um barbeiro esperando, acorda-o.

  public synchronized void signal(int idBarbeiro)
  {
    representacao.barbeiros_disp++;
    representacao.barbeiros[idBarbeiro]++;
    if (esperando > 0)
      notify();
  }

  // Metodo: cortaCabelo
  // Saida: true se foi possivel cortar o cabelo; false caso contrario.
  //        idBarbeiro: identificador do barbeiro que fez o corte.
  // Descricao: tenta atender um cliente, caso haja barbeiros ou lugares
  //            disponiveis. Se nao houver nenhum dos dois, vai embora. Se nao
  //            houver apenas barbeiros, espera ate um deles ficar disponivel.

  public boolean cortaCabelo(org.omg.CORBA.IntHolder idBarbeiro)
                 throws InterruptedException
  {
    int i;

    synchronized(this)
    {
      if ((representacao.clientes_esperando >= num_cadeiras) &&
          (representacao.barbeiros_disp == 0))
        return false;
      else
      {
        representacao.clientes_esperando++;
        while (representacao.barbeiros_disp == 0)
        {
          esperando++;
          try {
            wait();
          }
          catch(InterruptedException ie) {
            notify();
            throw ie;
          }
          finally {
            esperando--;
          }
        }
        i = representacao.barbeiros.length;
        while (i == representacao.barbeiros.length)
        { 
          for (i = 0; representacao.barbeiros[i] != 1 && i < representacao.barbeiros.length; i++);
          if (i < representacao.barbeiros.length)
          {
            representacao.barbeiros_disp--;
            representacao.barbeiros[i]--;
            idBarbeiro.value = i;
          }
        }
      }
    }
    representacao.clientes_esperando--;
    barbearia.signal(this);
    return true;
  }
}
