package gra;

public class UsuwajacaAkcja implements Akcja
{
    private Plansza plansza;
    UsuwajacaAkcja(Plansza p)
    {
        plansza = p;
    }

    public void wykonaj(Postać postać)
    {
        System.out.println("Usuwam postać z planszy");
        plansza.usuń(postać);
    }

}
