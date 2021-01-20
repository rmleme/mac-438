// Classe: ClienteSaiu.java
// Autores: Leonardo Sunao Miyagi                  Numero USP: 3116540
//          Rodrigo Mendes Leme                    Numero USP: 3151151
// Exercicio Programa 3                            Data: 29/06/2002
// Descricao: helper utilizado pela classe Barbearia para fazer acessos
//            sincronizados a condicao "cliente saiu".

public class ClienteSaiu
{
  private Barbearia barbearia;
  private Representacao representacao;
  private int esperando;

  public ClienteSaiu(Barbearia b, Representacao r)
  {
    barbearia     = b;
    representacao = r;
    esperando     = 0;
  }

  // Metodo: signal
  // Descricao: caso haja um cliente esperando, acorda-o.

  public synchronized void signal()
  {
    if (esperando > 0)
      notify();
  }

  // Metodo: corteTerminado
  // Descricao: o barbeiro aguarda ate que a porta de saida seja fechada pelo
  //            cliente.

  public void corteTerminado() throws InterruptedException
  {
    synchronized(this)
    {
      barbearia.signal(this);
      while (representacao.porta_aberta > 0)
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
    }
  }
}
