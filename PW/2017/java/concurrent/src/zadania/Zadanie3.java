package zadania;

import java.util.concurrent.CountDownLatch;

public class Zadanie3 {
    private static final int N_WIERSZY = 10;
    private static final int N_KOLUMN = 100;

    public static void main(final String[] args) {
        CountDownLatch [] zatrzaski = new CountDownLatch[N_WIERSZY];
        for (int i = 0; i < N_WIERSZY; i++) {
            zatrzaski[i] = new CountDownLatch(N_KOLUMN);
        }

        int [][] lista = new int [N_WIERSZY][N_KOLUMN];
        Thread [] robotnicy = new Thread[N_KOLUMN];

        for (int k = 0; k < N_KOLUMN; ++k) {
            robotnicy[k] = new Robotnik3(k, N_WIERSZY, lista, zatrzaski);
            robotnicy[k].start();
        }

        for (int w = 0; w < N_WIERSZY; ++w) {
            // Czekaj, aż wszyscy obliczą w-ty wiersz
            try {
                zatrzaski[w].await();
            } catch (Exception e) {
                e.printStackTrace();
            }
            int suma = 0;
            for (int k = 0; k < N_KOLUMN; ++k) {
                suma += lista[w][k];
            }
            System.out.println(w + " " + suma);

        }
    }
}

class Robotnik3 extends Thread {
    int poz;
    int [][] lista;
    int N_WIERSZY;
    CountDownLatch [] zatrzaski;

    public Robotnik3(int poz, int N_WIERSZY, int [][] lista, CountDownLatch [] zatrzaski) {
        this.poz = poz;
        this.lista = lista;
        this.N_WIERSZY = N_WIERSZY;
        this.zatrzaski = zatrzaski;
    }

    @Override
    public void run() {
        for (int w = 0; w < this.N_WIERSZY; w++) {
            lista[w][poz] = Macierz.wartość(w, poz);
            // generuj zdarzenie
            zatrzaski[w].countDown();
        }
    }
}
