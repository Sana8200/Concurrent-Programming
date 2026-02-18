package homework_4_monitors.P6_bear_honeybees;

import java.util.Random;

public class Bee implements Runnable{
    private final Pot pot;
    private final int beeId;
    private final Random random = new Random();
    
    public Bee(Pot pot, int beeId){
        this.pot = pot;
        this.beeId = beeId;
    }

    public void run(){
        while(true){
            try{
                Thread.sleep(random.nextInt(1000));   // gathering honey 
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt(); // Restore interrupt flag
                break; // Exit loop if interrupted
            }
            pot.addHoney(beeId); 
        }
    }
}