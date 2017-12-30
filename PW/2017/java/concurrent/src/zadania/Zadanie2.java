package zadania;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class Zadanie2 {
    private static final int N_WIERSZY = 10;
    private static final int N_KOLUMN = 100;

    public static void main(final String[] args) {
        CyclicBarrier bariera1 = new CyclicBarrier(N_KOLUMN + 1);    // +1 dla głównego wątku do obliczenia sumy
        CyclicBarrier bariera2 = new CyclicBarrier(N_KOLUMN + 1);    // +1 dla głównego wątku do obliczenia sumy

        List<Integer> lista = Collections.synchronizedList(new ArrayList<Integer>(Collections.nCopies(N_KOLUMN, 0)));
        Thread [] robotnicy = new Thread[N_KOLUMN];

        for (int k = 0; k < N_KOLUMN; ++k) {
            robotnicy[k] = new Robotnik2(k, lista, N_WIERSZY, bariera1, bariera2);
            robotnicy[k].start();
        }

        for (int w = 0; w < N_WIERSZY; ++w) {
            // Przejdź pierwszą barierę
            try {
                bariera1.await();
            } catch (Exception e) {
                e.printStackTrace();
            }

            // Wypisz sumę
            int suma = 0;
            for (int k = 0; k < N_KOLUMN; ++k) {
                suma += lista.get(k);
            }
            System.out.println(w + " " + suma);

            // Przejdź drugą barierę
            try {
                bariera2.await();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}

class Robotnik2 extends Thread {
    int poz;
    List<Integer> lista;
    int N_WIERSZY;
    CyclicBarrier b1;
    CyclicBarrier b2;

    public Robotnik2(int poz, List<Integer> lista, int N_WIERSZY, CyclicBarrier b1, CyclicBarrier b2) {
        this.poz = poz;
        this.lista = lista;
        this.N_WIERSZY = N_WIERSZY;
        this.b1 = b1;
        this.b2 = b2;
    }

    @Override
    public void run() {
        for (int w = 0; w < this.N_WIERSZY; w++) {
            lista.set(poz, Macierz.wartość(w, poz));
            try {
                b1.await();
            } catch (Exception e) {
                e.printStackTrace();
            }
            try {
                b2.await();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
