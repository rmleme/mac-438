// Classe: Barbeiro.java
// Autores: Leonardo Sunao Miyagi                  Numero USP: 3116540
//          Rodrigo Mendes Leme                    Numero USP: 3151151
// Exercicio Programa 3                            Data: 29/06/2002
// Descricao: classe que implementa as funcionalidades de um barbeiro, de
//            maneira concorrente.

public class Barbeiro extends Thread
{
  private Barbearia barbearia;
  private int idBarbeiro;

  public Barbeiro(Barbearia b, int i)
  {
    barbearia  = b;
    idBarbeiro = i;
  }

  // Metodo: run
  // Descricao: o barbeiro fica eternamente chamando clientes e cortando seus
  //            cabelos. O corte demora um tempo aleatorio.

  public void run()
  {
    java.util.Random aleat = new java.util.Random();

    while (true)
    {
      barbearia.proximoCliente(idBarbeiro);
      try {
        sleep(aleat.nextInt(1000));
      }
      catch(InterruptedException ie) {
        ie.printStackTrace();
      }
      barbearia.corteTerminado();
    }
  }  
}
