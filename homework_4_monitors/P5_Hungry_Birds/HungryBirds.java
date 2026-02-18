package homework_4_monitors.P5_Hungry_Birds;
/*
  javac *.java
  java HungryBirds [capacity] [babyBirds] 
*/
public class HungryBirds {
    public static void main(String[] args) {  
        int W = (args.length > 0) ? Integer.parseInt(args[0]) : 10;      // capacity
        int n = (args.length > 1) ? Integer.parseInt(args[1]) : 5;       // number of baby birds(processes)

        Dish dish = new Dish(W);

        Thread parentThread = new Thread(new ParentBird(dish));
        parentThread.start();


        Thread[] babies = new Thread[n];
        for (int i = 0; i < n; i++) {
            babies[i] = new Thread(new BabyBird(dish, i + 1));
            babies[i].start();
        }

        for (int i = 0; i < n; i++) {
            try {
                babies[i].join();
            } catch (InterruptedException e) {}
        }
    }
}