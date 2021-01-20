// Classe: Cliente.java
// Autores: Leonardo Sunao Miyagi                  Numero USP: 3116540
//          Rodrigo Mendes Leme                    Numero USP: 3151151
// Exercicio Programa 3                            Data: 29/06/2002
// Descricao: classe que implementa as funcionalidades de um cliente, de
//            maneira concorrente.

public class Cliente extends Thread
{
  private Barbearia barbearia;
  private int idCliente;

  public Cliente(Barbearia b, int i)
  {
    barbearia = b;
    idCliente = i;
  }

  // Metodo: run
  // Descricao: o cliente fica eternamente cortando o cabelo e esperando um
  //            tempo aleatorio ate o proximo corte.

  public void run()
  {
    java.util.Random aleat = new java.util.Random();

    while (true)
    {
      System.out.println("O cliente " + idCliente + " quer cortar o cabelo.");
      if (barbearia.cortaCabelo(idCliente))
      {
        System.out.println("O cliente " + idCliente + " terminou de ter o cabelo cortado.");
        System.out.println();
      }
      else
      {
        System.out.println("O cliente " + idCliente +
                           " nao conseguiu cortar o cabelo: barbearia lotada.");
        System.out.println();
      }
      try {
        sleep(aleat.nextInt(1000));
      }
      catch(InterruptedException ie) {
        ie.printStackTrace();
      }
    }
  }
}
