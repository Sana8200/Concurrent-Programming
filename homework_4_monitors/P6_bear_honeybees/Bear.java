package homework_4_monitors.P6_bear_honeybees;

public class Bear implements Runnable{
    private final Pot pot;

    public Bear(Pot pot){
        this.pot = pot;
    }

    public void run(){
        while(true){
            pot.eatHoney();
        }
    }
}