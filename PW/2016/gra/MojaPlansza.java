package gra;

import java.util.*;

public class MojaPlansza implements Plansza
{
    private boolean[][] zajete;
    private HashMap<Postać, Pair<Integer, Integer>> postaci;
    private Digraph<Postać> zaleznosci;

    /* metody pomocnicze */
    protected Postać dajPostac(int wiersz, int kolumna)
    {
        for (Map.Entry<Postać, Pair<Integer, Integer>> entry : postaci.entrySet())
        {
            if (
                entry.getValue().getFirst() <= wiersz &&
                entry.getValue().getFirst() + entry.getKey().dajWysokość() > wiersz &&
                entry.getValue().getSecond() <= kolumna &&
                entry.getValue().getSecond() + entry.getKey().dajSzerokość() > kolumna
            ) return entry.getKey();
        }
        return null;
    }
    protected Pair<Integer, Integer> tlumaczKierunek(Kierunek kierunek)
    {
        switch (kierunek)
        {
        case GÓRA:
            return new Pair<Integer, Integer>(-1, 0);
        case DÓŁ:
            return new Pair<Integer, Integer>(1, 0);
        case LEWO:
            return new Pair<Integer, Integer>(0, -1);
        case PRAWO:
            return new Pair<Integer, Integer>(0, 1);
        }
        return null;
    }

    /* mówi, czy na prostokącie o lewym górnym wierzchołku
      (x1, y1) i prawym dolnym (x2, y2) jest choć jedno
      zajęte pole */
    protected boolean zajetyProstokat(int x1, int y1, int x2, int y2)
    {
        // TODO arg checking
        for (int i = x1; i <= x2; i++)
            for (int j = y1; j <= y2; j++)
                if (zajete[i][j]) return true;
        return false;
    }

    protected void zajmijProstokat(int x1, int y1, int x2, int y2)
    {
        // TODO arg checking
        for (int i = x1; i <= x2; i++)
            for (int j = y1; j <= y2; j++)
                zajete[i][j] = true;
    }
    protected void zwolnijProstokat(int x1, int y1, int x2, int y2)
    {
        // TODO arg checking
        for (int i = x1; i <= x2; i++)
            for (int j = y1; j <= y2; j++)
                zajete[i][j] = false;
    }

    protected boolean pustePrzeciecie(
        int x1, int y1, int x2, int y2,
        int z1, int t1, int z2, int t2
    )
    {
        return  (x1 > z2) || (z1 > x2) || (y1 > t2) || (t1 > y2);
    }
    protected void znajdzZaleznosci(Postać postac, int x1, int y1, int x2, int y2)
    {
        zaleznosci.reset(postac);
        for (Map.Entry<Postać, Pair<Integer, Integer>> entry : postaci.entrySet())
        {
            if (!pustePrzeciecie(
                        x1, y1, x2, y2,
                        entry.getValue().getFirst(),
                        entry.getValue().getSecond(),
                        entry.getValue().getFirst() + entry.getKey().dajWysokość() - 1,
                        entry.getValue().getSecond() + entry.getKey().dajSzerokość() - 1
                    )) zaleznosci.add(postac, entry.getKey());
        }
    }

    // Sprawdza, czy dany punkt lezy na planszy
    protected void sprawdzPunkt(int x, int y) throws IllegalArgumentException
    {
        if (x < 0 || y < 0 ||
                x >= size().getFirst() || y >= size().getSecond())
            throw new IllegalArgumentException();
    }

    /* konstruktor */
    MojaPlansza(int wysokosc, int szerokosc)
    {
        zajete = new boolean[wysokosc][szerokosc];
        postaci = new HashMap<Postać, Pair<Integer, Integer>>();
        zaleznosci = new Digraph<Postać>();
    }

    /* metody interfejsu Plansza */
    public synchronized void postaw(Postać postac,
                                    int wiersz, int kolumna)
    throws InterruptedException
    {
        sprawdzPunkt(wiersz, kolumna);
        while (zajetyProstokat(
                    wiersz,
                    kolumna,
                    wiersz + postac.dajWysokość() - 1,
                    kolumna + postac.dajSzerokość() - 1
                ))
        {
            znajdzZaleznosci(
                postac,
                wiersz,
                kolumna,
                wiersz + postac.dajWysokość() - 1,
                kolumna + postac.dajSzerokość() - 1
            );
            wait();
        }

        zajmijProstokat(
            wiersz,
            kolumna,
            wiersz + postac.dajWysokość() - 1,
            kolumna + postac.dajSzerokość() - 1
        );
        postaci.put(
            postac,
            new Pair<Integer, Integer>(wiersz, kolumna)
        );
        zaleznosci.reset(postac);
    }

    public synchronized void przesuń(Postać postac, Kierunek kierunek)
    throws InterruptedException, DeadlockException
    {
        // TODO arg checking
        int doZajeciax1 = 0, doZajeciay1 = 0, doZajeciax2 = 0, doZajeciay2 = 0;
        int doZwolnieniax1 = 0, doZwolnieniay1 = 0, doZwolnieniax2 = 0, doZwolnieniay2 = 0;
        switch (kierunek)
        {
        case GÓRA:
            doZajeciax1 = postaci.get(postac).getFirst() - 1;
            doZajeciay1 = postaci.get(postac).getSecond();
            doZajeciax2 = postaci.get(postac).getFirst() - 1;
            doZajeciay2 = postaci.get(postac).getSecond() + postac.dajSzerokość() - 1;
            doZwolnieniax1 = postaci.get(postac).getFirst() + postac.dajWysokość() - 1;
            doZwolnieniay1 = doZajeciay1;
            doZwolnieniax2 = doZwolnieniax1;
            doZwolnieniay2 = doZajeciay2;
            break;
        case DÓŁ:
            doZajeciax1 = postaci.get(postac).getFirst() + postac.dajWysokość();
            doZajeciay1 = postaci.get(postac).getSecond();
            doZajeciax2 = postaci.get(postac).getFirst() + postac.dajWysokość();
            doZajeciay2 = postaci.get(postac).getSecond() + postac.dajSzerokość() - 1;
            doZwolnieniax1 = postaci.get(postac).getFirst();
            doZwolnieniay1 = doZajeciay1;
            doZwolnieniax2 = doZwolnieniax1;
            doZwolnieniay2 = doZajeciay2;
            break;
        case LEWO:
            doZajeciax1 = postaci.get(postac).getFirst();
            doZajeciay1 = postaci.get(postac).getSecond() - 1;
            doZajeciax2 = postaci.get(postac).getFirst() + postac.dajWysokość() - 1;
            doZajeciay2 = postaci.get(postac).getSecond() - 1;
            doZwolnieniax1 = doZajeciax1;
            doZwolnieniay1 = postaci.get(postac).getSecond() + postac.dajSzerokość() - 1;
            doZwolnieniax2 = doZajeciax2;
            doZwolnieniay2 = doZwolnieniay1;
            break;
        case PRAWO:
            doZajeciax1 = postaci.get(postac).getFirst();
            doZajeciay1 = postaci.get(postac).getSecond() + postac.dajSzerokość();
            doZajeciax2 = postaci.get(postac).getFirst() + postac.dajWysokość() - 1;
            doZajeciay2 = postaci.get(postac).getSecond() + postac.dajSzerokość();
            doZwolnieniax1 = doZajeciax1;
            doZwolnieniay1 = postaci.get(postac).getSecond();
            doZwolnieniax2 = doZajeciax2;
            doZwolnieniay2 = doZwolnieniay1;
            break;
        }
        sprawdzPunkt(doZajeciax1, doZajeciay1);
        sprawdzPunkt(doZajeciax2, doZajeciay2);
        while (zajetyProstokat(
                    doZajeciax1,
                    doZajeciay1,
                    doZajeciax2,
                    doZajeciay2
                ))
        {
            znajdzZaleznosci(
                postac,
                doZajeciax1,
                doZajeciay1,
                doZajeciax2,
                doZajeciay2
            );
            if (zaleznosci.isCyclicRec())
            {
                notifyAll();
                throw new DeadlockException();
            }
            wait();
        }

        zajmijProstokat(
            doZajeciax1,
            doZajeciay1,
            doZajeciax2,
            doZajeciay2
        );

        zwolnijProstokat(
            doZwolnieniax1,
            doZwolnieniay1,
            doZwolnieniax2,
            doZwolnieniay2
        );

        postaci.put(
            postac,
            new Pair<Integer, Integer>(
                postaci.get(postac).getFirst() + tlumaczKierunek(kierunek).getFirst(),
                postaci.get(postac).getSecond() + tlumaczKierunek(kierunek).getSecond()
            )
        );

        zaleznosci.reset(postac); // po wykonaniu ruchu na nikogo nie czekamy
        notifyAll();
    }

    public synchronized void usuń(Postać postac)
    {
        // TODO arg checking
        Pair<Integer, Integer> wspolrzedne = postaci.get(postac);
        zwolnijProstokat(
            wspolrzedne.getFirst(),
            wspolrzedne.getSecond(),
            wspolrzedne.getFirst() + postac.dajWysokość() - 1,
            wspolrzedne.getSecond() + postac.dajSzerokość() - 1
        );
        postaci.remove(postac);
        notifyAll();
    }

    public synchronized void sprawdź(int wiersz, int kolumna,
                                      Akcja jesliZajete, Runnable jesliWolne)
    {
        sprawdzPunkt(wiersz, kolumna);
        if (zajete[wiersz][kolumna])
        {
            Postać postac = dajPostac(wiersz, kolumna);
            jesliZajete.wykonaj(postac);
        }
        else
        {
            jesliWolne.run();
        }
    }

    public String toString()
    {
        String result = "";
        for (int i = 0; i < zajete.length; i++)
        {
            for (int j = 0; j < zajete[i].length; j++)
            {
                result += zajete[i][j] ? 'x' : 'o';
                result += j == zajete[i].length - 1 ? "" : " ";
            }
            if (i < zajete.length - 1) result += '\n';
        }
        return result + '\n';
    }

    public Pair<Integer, Integer> size()
    {
        return new Pair<Integer, Integer>(zajete.length, zajete[0].length);
    }
}
