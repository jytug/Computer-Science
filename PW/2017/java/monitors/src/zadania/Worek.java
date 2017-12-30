package zadania;

public interface Worek<T> {

    void włóż(T wartość);
    void wyjmij(T wartość) throws InterruptedException;

}