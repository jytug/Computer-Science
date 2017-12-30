package zadania;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class Zadanie1 {
    private static final int N_WIERSZY = 10;
    private static final int N_KOLUMN = 100;

    public static void main(final String[] args) {
        CyclicBarrier bariera1 = new CyclicBarrier(N_KOLUMN + 1);    // +1 dla głównego wątku do obliczenia sumy
        CyclicBarrier bariera2 = new CyclicBarrier(N_KOLUMN + 1);    // +1 dla głównego wątku do obliczenia sumy

        int [] lista = new int[N_KOLUMN];
        Thread [] robotnicy = new Thread[N_KOLUMN];

        for (int k = 0; k < N_KOLUMN; ++k) {
            robotnicy[k] = new Robotnik1(k, lista, N_WIERSZY, bariera1, bariera2);
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
                suma += lista[k];
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

class Robotnik1 extends Thread {
    int poz;
    int [] lista;
    int N_WIERSZY;
    CyclicBarrier b1;
    CyclicBarrier b2;

    public Robotnik1(int poz, int [] lista, int N_WIERSZY, CyclicBarrier b1, CyclicBarrier b2) {
        this.poz = poz;
        this.lista = lista;
        this.N_WIERSZY = N_WIERSZY;
        this.b1 = b1;
        this.b2 = b2;
    }

    @Override
    public void run() {
        for (int w = 0; w < this.N_WIERSZY; w++) {
            lista[poz] = Macierz.wartość(w, poz);
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
