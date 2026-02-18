package homework_4_monitors.P5_Hungry_Birds;

public class ParentBird implements Runnable {
    private final Dish dish;
    
    public ParentBird(Dish dish){
        this.dish = dish;
    }

    public void run(){
        while(true){
            dish.fillDish();
        }
    }
}