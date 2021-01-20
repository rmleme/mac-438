// Classe: Principal.java
// Autores: Leonardo Sunao Miyagi                  Numero USP: 3116540
//          Rodrigo Mendes Leme                    Numero USP: 3151151
// Exercicio Programa 3                            Data: 29/06/2002
// Descricao: instancia um pseudo-monitor barbearia e inicia a simulacao.

public class Principal
{
  public static void main(String[] args)
  {
    int num_cad,
        num_cli,
        num_barb;

    if (args.length == 3)
    {
      num_cad  = Integer.parseInt(args[0]);
      num_cli  = Integer.parseInt(args[1]);
      num_barb = Integer.parseInt(args[2]);
      Barbearia barbearia = new Barbearia(num_cad,num_cli,num_barb);
      barbearia.abreBarbearia();
    }
    else
    {
      System.out.println("Erro: numero de argumentos invalido.");
      System.out.println();
    }
  }
}
