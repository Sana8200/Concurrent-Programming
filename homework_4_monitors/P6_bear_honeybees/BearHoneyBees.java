package homework_4_monitors.P6_bear_honeybees;
/*
  javac *.java
  java BearHoneyBees [capacity] [bees] 
*/
public class BearHoneyBees{
    public static void main(String[] args) {  
        int H = (args.length > 0) ? Integer.parseInt(args[0]) : 10;      // capacity
        int n = (args.length > 1) ? Integer.parseInt(args[1]) : 5;       // number of honeybees(processes)

        Pot pot = new Pot(H);

        Thread[] bees = new Thread[n];
        for(int i = 0; i < n; i++){
            bees[i] = new Thread(new Bee(pot, i + 1));
            bees[i].start();
        }

        Thread bear = new Thread(new Bear(pot));
        bear.start();


        for (int i = 0; i < n; i++) {
            try {
                bees[i].join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt(); // Restore interrupt flag
            }
        }
    }
}