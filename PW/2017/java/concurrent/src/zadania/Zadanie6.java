package zadania;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;


public class Zadanie6 {
    private static final int N_WIERSZY = 10;
    private static final int N_KOLUMN = 100;

    private static class WartoscMacierzy implements Callable<Integer> {

        private final int k, w;

        private WartoscMacierzy(final int w, final int k) {
            this.w = w;
            this.k = k;
        }

        @Override
        public Integer call() {
            return Macierz.wartość(w, k);
        }

    }

    public static void main(final String[] args) {
        final ExecutorService pula = Executors.newFixedThreadPool(4);
        Future<Integer> [][] macierz_leniwa = new Future[N_WIERSZY][N_KOLUMN];

        for (int w = 0; w < N_WIERSZY; ++w) {
            for (int k = 0; k < N_KOLUMN; ++k) {
                macierz_leniwa[w][k] = pula.submit(new WartoscMacierzy(w, k));
            }
        }

        for (int w = 0; w < N_WIERSZY; ++w) {
            int suma = 0;
            for (int k = 0; k < N_KOLUMN; ++k) {
                try {
                    suma += macierz_leniwa[w][k].get();
                } catch (Exception e) {
                    // Zakoncz dzialanie
                    e.printStackTrace();
                    Thread.currentThread().interrupt();
                }
            }
            System.out.println(w + " " + suma);
        }

        pula.shutdown();
    }
}
