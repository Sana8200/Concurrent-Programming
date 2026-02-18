package homework_4_monitors.Problem_5;
/* Dish monitor */

import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;

public class Dish {
    private int worms;
    private final int capacity;
    private final ReentrantLock lock = new ReentrantLock();
    private final Condition empty = lock.newCondition();
    private final Condition full = lock.newCondition();

    public Dish(int capacity){
        this.capacity = capacity;
        this.worms = capacity;     // starts with a full dish
    }

    public void takeWorms(int birdId){
        lock.lock();
        try{
            while(worms == 0){
                System.out.println("Bird " + birdId + " is waiting for worms.");
                empty.signal();
                full.await();
            } 
            worms--;
            System.out.println("Bird " + birdId + " took a worm. Worms left: " + worms);
        } catch (InterruptedException e) {}
        finally{
            lock.unlock();
        }
    }

    public void fillDish(){
        lock.lock();
        try{
            while(worms > 0){
                empty.await();     // sleep until a baby bird notifies dish is empty
            }
            worms = capacity;
            System.out.println("Parent refilled dish with fresh " + capacity + " worms!!!");
            full.signalAll();     // Wake all waiting babies
        } catch(InterruptedException e){}
        finally {
            lock.unlock();
        }
    }
}