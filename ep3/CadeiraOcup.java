// Classe: CadeiraOcup.java
// Autores: Leonardo Sunao Miyagi                  Numero USP: 3116540
//          Rodrigo Mendes Leme                    Numero USP: 3151151
// Exercicio Programa 3                            Data: 29/06/2002
// Descricao: helper utilizado pela classe Barbearia para fazer acessos
//            sincronizados a condicao "cadeira ocupada".

public class CadeiraOcup
{
  private Barbearia barbearia;
  private Representacao representacao;
  private int esperando;

  public CadeiraOcup(Barbearia b, Representacao r)
  {
    barbearia     = b;
    representacao = r;
    esperando     = 0;
  }

  // Metodo: signal
  // Descricao: caso haja barbeiro esperando um cliente, acorde aquele.

  public synchronized void signal()
  {
    representacao.cadeira++;
    if (esperando > 0)
      notify();
  }

  // Metodo: proximoCliente
  // Entrada: idBarbeiro: identificador do barbeiro que chamou o cliente.
  // Descricao: o barbeiro aguarda ate que um cliente chegue.

  public void proximoCliente(int idBarbeiro) throws InterruptedException
  {
    synchronized(this)
    {
      barbearia.signal(this,idBarbeiro);
      while (representacao.cadeira == 0)
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
      representacao.cadeira--;
    }
  }
}
