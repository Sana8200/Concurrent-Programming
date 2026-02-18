package homework_4_monitors.P5_Hungry_Birds;
/* Dish monitor - shared by parent and baby birds (critical section for producer and consumers)
   Fairness: Not strictly fair, with ReentrantLock(false) which is the default, there is no guarantee about which waiting baby 
   bird will be woken next. A bird could starve if other birds gets the lock again and again earlier than that bird
   If we use ReentrantLock(true), we could provid FIFO ordering, making the solution strictly fair
*/
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

    // Called by baby birds to take worms from the dish (consumer)
    public void takeWorms(int birdId){
        lock.lock();
        try{
            while(worms == 0){
                System.out.println("Bird " + birdId + " is waiting for worms.");
                empty.signal();   // wake parent that dish is empty to fill the emptry dish 
                full.await();     // wait until parent refills the dish
            } 
            worms--;
            System.out.println("Bird " + birdId + " took a worm. Worms left: " + worms);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
        finally{
            lock.unlock();
        }
    }

    // Called by the parent bird to fill the dish (producer)
    public void fillDish(){
        lock.lock();
        try{
            while(worms > 0){
                empty.await();     // parent waits until dish is empty (signaled by a baby bird)
            } 
            System.out.println("Parent is gathering worms...");
            try{
                Thread.sleep(1000);   // gattering time for parent bird 
            } catch (InterruptedException e) {}

            worms = capacity;
            System.out.println("Parent refilled dish with fresh " + capacity + " worms!!!");
            full.signalAll();     // Wake all waiting babies
        } catch(InterruptedException e){
            Thread.currentThread().interrupt(); // Restore interrupt flag
        }
        finally {
            lock.unlock();
        }
    }
}