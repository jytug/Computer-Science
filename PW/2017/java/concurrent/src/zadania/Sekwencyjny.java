package zadania;

public class Sekwencyjny {

    private static final int N_WIERSZY = 10;
    private static final int N_KOLUMN = 100;

    public static void main(final String[] args) {
        for (int w = 0; w < N_WIERSZY; ++w) {
            int suma = 0;
            for (int k = 0; k < N_KOLUMN; ++k) {
                suma += Macierz.wartość(w, k);
            }
            System.out.println(w + " " + suma);
        }
    }

}
