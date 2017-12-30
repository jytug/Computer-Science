package zadania;

import java.util.HashMap;

public class HashWorek<T> implements Worek<T> {
    private HashMap<T, Integer> wartości = new HashMap<T, Integer>();

    public synchronized void włóż(T wartość) {
        Integer aktualna_liczba = wartości.containsKey(wartość) ? wartości.get(wartość) : 0;
        wartości.put(wartość, aktualna_liczba + 1);
        if (aktualna_liczba == 0)
            notifyAll();
    }

    public synchronized void wyjmij(T wartość) throws InterruptedException {
        while (!wartości.containsKey(wartość)) {
            wait();
        }
        Integer aktualna_liczba = wartości.get(wartość);
        if (aktualna_liczba == 1) {
            wartości.remove(wartość);
        } else {
            wartości.put(wartość, 0);
        }
    }
}
