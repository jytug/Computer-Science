package zadania;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class Zadanie4 {
    private static final int N_WIERSZY = 10;
    private static final int N_KOLUMN = 100;

    public static void main(final String[] args) {
        BlockingQueue<Integer> [] kolejki = new BlockingQueue[N_WIERSZY];
        for (int i = 0; i < N_WIERSZY; i++) {
            kolejki[i] = new LinkedBlockingQueue<Integer>();
        }

        Thread [] robotnicy = new Thread[N_KOLUMN];

        for (int k = 0; k < N_KOLUMN; ++k) {
            robotnicy[k] = new Robotnik4(k, N_WIERSZY, kolejki);
            robotnicy[k].start();
        }

        for (int w = 0; w < N_WIERSZY; ++w) {
            int suma = 0;
            for (int k = 0; k < N_KOLUMN; ++k) {
                try {
                    suma += kolejki[w].take();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            System.out.println(w + " " + suma);

        }
    }
}

class Robotnik4 extends Thread {
    int poz;
    int N_WIERSZY;
    BlockingQueue [] kolejki;

    public Robotnik4(int poz, int N_WIERSZY, BlockingQueue [] kolejki) {
        this.poz = poz;
        this.N_WIERSZY = N_WIERSZY;
        this.kolejki = kolejki;
    }

    @Override
    public void run() {
        for (int w = 0; w < this.N_WIERSZY; w++) {
            try {
                kolejki[w].put(Macierz.wartość(w, poz));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

