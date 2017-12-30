package zadania;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class Zadanie5 {
    private static final int N_WIERSZY = 10;
    private static final int N_KOLUMN = 100;

    public static void main(final String[] args) {
        ConcurrentMap<Integer, BlockingQueue<Integer>> kolejki =
                new ConcurrentHashMap<Integer, BlockingQueue<Integer>>();

        Thread [] robotnicy = new Thread[N_KOLUMN];

        for (int k = 0; k < N_KOLUMN; ++k) {
            robotnicy[k] = new Robotnik5(k, N_WIERSZY, kolejki);
            robotnicy[k].start();
        }

        for (int w = 0; w < N_WIERSZY; ++w) {
            int suma = 0;
            kolejki.putIfAbsent(w, new LinkedBlockingQueue<Integer>());
            for (int k = 0; k < N_KOLUMN; ++k) {
                try {
                    suma += kolejki.get(w).take();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            System.out.println(w + " " + suma);

        }
    }
}

class Robotnik5 extends Thread {
    int poz;
    int N_WIERSZY;
    ConcurrentMap<Integer, BlockingQueue<Integer>> kolejki;

    public Robotnik5(int poz, int N_WIERSZY, ConcurrentMap<Integer, BlockingQueue<Integer>> kolejki) {
        this.poz = poz;
        this.N_WIERSZY = N_WIERSZY;
        this.kolejki = kolejki;
    }

    @Override
    public void run() {
        for (int w = 0; w < this.N_WIERSZY; w++) {
            kolejki.putIfAbsent(w, new LinkedBlockingQueue<Integer>());
            try {
                kolejki.get(w).put(Macierz.wartość(w, poz));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
