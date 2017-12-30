package zadania;

import java.util.HashMap;
import java.util.Map;

public class TestWorek {
    static final int LICZBA = 8;
    static Worek<Integer> worek = new HashWorek();

    public static void main(String [] args) {
        Thread[] testerzy = new Thread[2 * LICZBA];
        System.out.println("Odpalam " + 2 * LICZBA +
                " wątków testujących. Wątki będą wkładać wartości z zakresu 0-" + LICZBA);
        for (int i = 0; i < 2 * LICZBA; i++) {
            testerzy[i] = new WorekTester(i % LICZBA);
            testerzy[i].start();
        }
        for (int i = 0; i < 2 * LICZBA; i++) {
            try {
                testerzy[i].join();
            } catch (InterruptedException e) {}
        }
        System.out.println("Wszystkie wątki skończyły pracę");
    }

    private static class WorekTester extends Thread {
        private Integer numer;

        WorekTester(int numer) {
            this.numer = new Integer(numer);
        }

        @Override
        public void run() {
            System.out.println("Wątek " + Thread.currentThread().getName() + " wkłada wartość " + numer + " do worka");
            worek.włóż(numer);
            try {
                Thread.sleep((long)(Math.random() * 1000));
                System.out.println("Wątek " + Thread.currentThread().getName() +
                        " wyjmuje wartość " + (LICZBA - numer - 1) + " z worka");
                worek.wyjmij(LICZBA - numer - 1);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
