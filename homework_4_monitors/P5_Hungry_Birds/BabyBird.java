package homework_4_monitors.P5_Hungry_Birds;

import java.util.Random;

public class BabyBird implements Runnable {
    private final Dish dish;
    private final int birdId;
    private final Random random = new Random();

    public BabyBird(Dish dish, int birdId) {
        this.dish = dish;
        this.birdId = birdId;
    }

    public void run(){
        while(true){
            dish.takeWorms(birdId);
            try{
                System.out.println("Bird " + birdId + " is eating...");
                Thread.sleep(random.nextInt(1000));
            } catch(InterruptedException e){
                Thread.currentThread().interrupt(); // Restore flag
                break; // Exit loop if interrupted
            } 
        }
    }
}
