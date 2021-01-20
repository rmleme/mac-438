// Classe: PortaAberta.java
// Autores: Leonardo Sunao Miyagi                  Numero USP: 3116540
//          Rodrigo Mendes Leme                    Numero USP: 3151151
// Exercicio Programa 3                            Data: 29/06/2002
// Descricao: helper utilizado pela classe Barbearia para fazer acessos
//            sincronizados a condicao "porta aberta".

public class PortaAberta
{
  private Barbearia barbearia;
  private Representacao representacao;
  private int esperando;

  public PortaAberta(Barbearia b, Representacao r)
  {
    barbearia     = b;
    representacao = r;
    esperando     = 0;
  }

  // Metodo: signal
  // Descricao: caso haja um cliente esperando, acorda-o.

  public synchronized void signal()
  {
    representacao.porta_aberta++;
    if (esperando > 0)
      notify();
  }

  // Metodo: cortaCabelo
  // Descricao: o cliente aguarda ate que a porta seja aberta pelo barbeiro.

  public void cortaCabelo() throws InterruptedException
  {
    synchronized(this)
    {
      while (representacao.porta_aberta == 0)
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
      representacao.porta_aberta--;
    }
    barbearia.signal(this);
  }
}
