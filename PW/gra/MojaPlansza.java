package gra;

import java.util.*;

public class MojaPlansza implements Plansza {
    private boolean[][] zajete;
    private HashMap<Postać, Pair<Integer, Integer>> postaci;

    /* metody pomocnicze */

    /* mówi, czy na prostokącie o lewym górnym wierzchołku
      (x1, y1) i prawym dolnym (x2, y2) jest choć jedno
      zajęte pole */
    protected boolean zajetyProstokat(int x1, int y1, int x2, int y2) {
        // TODO arg checking
        for (int i = x1; i <= x2; i++)
            for (int j = y1; j <= y2; j++)
                if (zajete[i][j]) return true;
        return false;
    }

    protected void zajmijProstokat(int x1, int y1, int x2, int y2) {
      // TODO arg checking
      for (int i = x1; i <= x2; i++)
          for (int j = y1; j <= y2; j++)
              zajete[i][j] = true;
    }
    protected void zwolnijProstokat(int x1, int y1, int x2, int y2) {
      // TODO arg checking
      for (int i = x1; i <= x2; i++)
          for (int j = y1; j <= y2; j++)
              zajete[i][j] = false;
    }

    /* konstruktory */
    MojaPlansza(int wysokosc, int szerokosc) {
        zajete = new boolean[wysokosc][szerokosc];
        postaci = new HashMap<Postać, Pair<Integer, Integer>>();
    }

    /* metody interfejsu Plansza */
    public synchronized void postaw(Postać postac,
        int wiersz, int kolumna)
            throws InterruptedException {
        while (zajetyProstokat(
            wiersz,
            kolumna,
            wiersz + postac.dajWysokość(),
            kolumna + postac.dajSzerokość()
        )) wait();

        zajmijProstokat(
            wiersz,
            kolumna,
            wiersz + postac.dajWysokość(),
            kolumna + postac.dajSzerokość()
        );
        postaci.put(postac, new Pair<Integer, Integer>(wiersz, kolumna));

    }

    public void przesuń(Postać postac, Kierunek kierunek) {

    }

    public void usuń(Postać postac) {

    }

    public void sprawdź(int wiersz, int kolumna,
            Akcja jesliZajete, Runnable jesliWolne) {

    }

    public String toString() {
        String result = "";
        for (int i = 0; i < zajete.length; i++) {
            for (int j = 0; j < zajete[i].length; j++) {
                result += zajete[i][j];
                result += j == zajete[i].length - 1 ? "" : " ";
            }
            if (i < zajete.length - 1) result += '\n';
        }
        return result;
    }
}
