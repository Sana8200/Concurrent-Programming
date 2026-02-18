Homework 4: Programming with Concurrent Objects (Monitors)
Implement, using monitors, the following problems from the previous homework on Semaphores. You are not allowed to use semaphores in this homework.
5. The Hungry Birds Problem (20 points)
Given are n baby birds and one parent bird. The baby birds eat out of a common dish that initially contains W worms. Each baby bird repeatedly takes a worm, eats it, sleeps for a while, takes another worm, and so on. When the dish is empty, the baby bird who discovers the empty dish chirps loudly to awaken the parent bird. The parent bird flies off, gathers W more worms, puts them in the dish, and then waits for the dish to be empty again. This pattern repeats forever.

Represent the birds as concurrent processes (i.e., an array of "baby bird" processes and a "parent bird" process) and the dish as a concurrent object (a monitor) that can be accessed by at most one bird at a time. 

Develop a monitor (with condition variables) to synchronize the actions of the birds, i.e., develop a monitor that represents the dish. Define the monitor's operations and their implementation. Implement a multithreaded application in Java or C++ to simulate the actions of the birds represented as concurrent threads and the dish represented as the developed monitor. Is your solution fair? Explain in comments in the source code.

6. The Bear and the Honeybees Problem (20 points)
Given are n honeybees and a hungry bear. They share a pot of honey. The pot is initially empty; its capacity is H portions of honey. The bear sleeps until the pot is full, eats all the honey, and goes back to sleep. Each bee repeatedly gathers one portion of honey and puts it in the pot; the bee who fills the pot awakens the bear.

Represent the bear and honeybees as concurrent processes or threads (i.e., a "bear" process and an array of "honeybee" processes) and the honey pot as a critical resource (a monitor) that can be accessed by at most one process at a time (either by the "bear" process or by one of the "honeybee" processes). 

Develop a monitor (with condition variables) to synchronize the actions of the bear and honeybees, i.e., develop a monitor representing the pot of honey. Define the monitor's operations and their implementation. Implement a multithreaded application in Java or C++ to simulate the actions of the bear and honeybees represented as concurrent threads and the pot represented as the monitor. Is your solution fair? Explain in comments in the source code.