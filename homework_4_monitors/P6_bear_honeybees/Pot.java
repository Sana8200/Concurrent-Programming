package homework_4_monitors.P6_bear_honeybees;
/* Pot Monitor - honey pot shared by the bear and the bees (critical section for producers and consumer)
   Farirness: Not Strictly fair. It's using default ReentrantLock(false),there is no guarantee
   about which waiting bee gets to add honey next. There is a possiblity that a bee starves since other bees gets the lock
   faster or first repeatedly. By using ReentrantLock(true), we could provide FIFO ordering, making the solution strictly fair
*/
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;

public class Pot {
    private int honeyPortions;
    private final int capacity;
    private final ReentrantLock lock = new ReentrantLock();
    private final Condition empty = lock.newCondition();
    private final Condition full = lock.newCondition();

    public Pot(int capacity){
        this.capacity = capacity;
        this.honeyPortions = 0;    // starts with an empty pot
    }   
 
    public void addHoney(int beeId){
        lock.lock();
        try{
            while(honeyPortions == capacity){
                System.out.println("Bee " + beeId + " is waiting for the pot to be empty by the hungry bear...");
                empty.await();     // wait until the pot is empty
            }
            honeyPortions++;
            System.out.println("Bee " + beeId + " added honey. Portions in pot: " + honeyPortions);
            if(honeyPortions == capacity){
                System.out.println("Pot is full! Bee " + beeId + " wakes the bear.");
                full.signal();     // signal the bear that the pot is full
            } 
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt(); 
        }
        finally{
            lock.unlock();
        }
    } 


    public void eatHoney(){
        lock.lock();
        try{
            while(honeyPortions < capacity){
                System.out.println("Bear is waiting for the pot to be full...");
                full.await();     // wait until the pot is full
            } 
            System.out.println("Bear is eating honey...");
            Thread.sleep(1000);   // eating time for bear
            honeyPortions = 0;    // bear eats all the honey in the pot

            System.out.println("Bear finished eating and empties the pot.");
            empty.signalAll();     // wake all waiting bees to start filling the pot again
        } catch(InterruptedException e){
            Thread.currentThread().interrupt(); 
        }
        finally {
            lock.unlock();
        }
    }
}
