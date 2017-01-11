package gra;

public class MojaPostać implements Postać
{
    private final int wysokosc;
    private final int szerokosc;
    MojaPostać(int wys, int szer)
    {
        wysokosc = wys;
        szerokosc = szer;
    }
    public int dajWysokość()
    {
        return wysokosc;
    };

    public int dajSzerokość()
    {
        return szerokosc;
    };

}
