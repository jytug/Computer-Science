package gra;

public class MojaPlanszaTest
{
    static MojaPlansza plansza;

    public static class JedenRuch extends Thread
    {
        int wiersz;
        int kolumna;
        Kierunek kierunek;
        JedenRuch(int w, int k, Kierunek kier)
        {
            wiersz = w;
            kolumna = k;
            kierunek = kier;
        }
        public void run()
        {
            MojaPostać pionek = new MojaPostać(1,1);
            try
            {
                plansza.postaw(pionek, wiersz, kolumna);
                System.out.println("Czekam dwie sekundy...");
                sleep(2000);
                System.out.println("Wykonuję ruch");
                plansza.przesuń(pionek, kierunek);
            }
            catch (DeadlockException e)
            {
                System.out.println("Rzucono [" + e + "]. OK");
            }
            catch (Exception e)
            {
                System.out.println("Inny wyjątek: " + e + ". Błąd");
            }
        }
    }

    public static void main(String args[])
    {
        plansza = new MojaPlansza(3, 3);
        System.out.println("Rozmiar planszy: " + plansza.size());
        System.out.println(plansza);
        System.out.println("==== TEST 1 ====");
        System.out.println("Testuję dwa pionki chodzące przez środek planszy...");
        Thread t1 = new Thread()
        {
            public void run()
            {
                MojaPostać pionek = new MojaPostać(1,1);
                try
                {
                    plansza.postaw(pionek, 0, 1);
                    for (int j = 0; j < 100; j++)
                    {
                        for (int i = 0; i < plansza.size().getSecond() - 1; ++i)
                            plansza.przesuń(pionek, Kierunek.DÓŁ);
                        for (int i = 0; i < plansza.size().getSecond() - 1; ++i)
                            plansza.przesuń(pionek, Kierunek.GÓRA);
                    }
                }
                catch (Exception e)
                {
                    System.out.println(e);
                }
                plansza.usuń(pionek);
                System.out.println("Wątek 1 skończył");
            }
        };
        Thread t2 = new Thread()
        {
            public void run()
            {
                MojaPostać pionek = new MojaPostać(1,1);
                try
                {
                    plansza.postaw(pionek, 1, 0);
                    for (int j = 0; j < 100; j++)
                    {
                        for (int i = 0; i < plansza.size().getSecond() - 1; ++i)
                            plansza.przesuń(pionek, Kierunek.PRAWO);
                        for (int i = 0; i < plansza.size().getSecond() - 1; ++i)
                            plansza.przesuń(pionek, Kierunek.LEWO);
                    }
                }
                catch (Exception e)
                {
                    System.out.println(e);
                }
                plansza.usuń(pionek);
                System.out.println("Wątek 2 skończył");
            }
        };
        try
        {
            t1.start();
            t2.start();

            t1.join();
            t2.join();
        }
        catch (Exception e) {}
        System.out.println("OK");
        System.out.println("===============\n");
        System.out.println("==== TEST 2 ====");
        t1 = new Thread()
        {
            public void run()
            {
                MojaPostać pionek = new MojaPostać(3,2);
                System.out.println("Stawiam duży klocek i próbuję przesunąć w prawo");
                try
                {
                    plansza.postaw(pionek, 0, 0);
                    sleep(1000);
                    plansza.przesuń(pionek, Kierunek.PRAWO);
                    System.out.println("Duży klocek przesunął się\nOK");
                    System.out.println(plansza);
                    plansza.usuń(pionek);
                }
                catch (Exception e)
                {
                    System.out.println(e);
                }
            }
        };
        t2 = new Thread()
        {
            public void run()
            {
                MojaPostać pionek = new MojaPostać(1,1);
                System.out.println("Stawiam mały klocek blokujący duży klocek");
                try
                {
                    plansza.postaw(pionek, 1, 2);
                    System.out.println("Czekam dwie sekundy...");
                    sleep(2000);
                    System.out.println("Usuwam mały klocek");
                    plansza.usuń(pionek);
                }
                catch (InterruptedException e)
                {
                    System.out.println("Wyjątek");
                }
            }
        };
        try
        {
            t1.start();
            t2.start();

            t1.join();
            t2.join();
        }
        catch (Exception e) {}
        System.out.println("================\n");
        System.out.println("==== TEST 3 ====");
        t1 = new Thread()
        {
            public void run()
            {
                MojaPostać pionek = new MojaPostać(
                    plansza.size().getFirst(),
                    plansza.size().getSecond()
                );
                System.out.println("Stawiam klocek na całą planszę");
                try
                {
                    plansza.postaw(pionek, 0, 0);
                    System.out.println("Sprawdzam dwukrotnie dowolne pole. Jeśli jest "+
                                       "zajęte, usuwam postać, w przeciwnym razie wypisuję odpowiedni "+
                                       "komunikat:");
                    for (int i = 0; i < 2; i++)
                    {
                        plansza.sprawdź(
                            0,
                            0,
                            new UsuwajacaAkcja(plansza),
                            new Runnable()
                        {
                            public void run()
                            {
                                System.out.println("To pole jest wolne");
                            }
                        }
                        );
                    }
                }
                catch (InterruptedException e)
                {
                    System.out.println("Wyjątek");
                }
            }
        };
        try
        {
            t1.start();

            t1.join();
        }
        catch (Exception e) {}
        System.out.println("================\n");
        System.out.println("==== TEST 4 ====");
        System.out.println("Testuję zakleszczenie...");
        t1 = new JedenRuch(0, 0, Kierunek.PRAWO);
        t2 = new JedenRuch(0, 1, Kierunek.DÓŁ);
        Thread t3 = new JedenRuch(1, 1, Kierunek.LEWO);
        Thread t4 = new JedenRuch(1, 0, Kierunek.GÓRA);

        try
        {
            t1.start();
            t2.start();
            t3.start();
            t4.start();

            t1.join();
            t2.join();
            t3.join();
            t4.join();
        }
        catch (Exception e) {}
        System.out.println("================");

    }
}
