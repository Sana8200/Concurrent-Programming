/*
 * Problem 2: Distributed Pairing (Peer-to-Peer Application)
 * Course: ID1217 - Concurrent Programming
 * 
 * ALGORITHM EXPLANATION:
 * ======================
 * Peer-to-peer pairing where students coordinate WITHOUT a central server.
 * 
 * Processes:
 * - Process 0: TEACHER (only initiates, does NOT manage pairing)
 * - Processes 1 to n: STUDENTS (pair with each other autonomously)
 * 
 * Algorithm (Randomized Token-Based Pairing):
 * ------------------------------------------
 * 1. Teacher picks a RANDOM student and gives them the "turn token"
 * 2. Student with token:
 *    - Randomly selects an unpaired student
 *    - Sends a proposal
 *    - If accepted, both are paired
 *    - Passes token to next student in ring
 * 3. Process continues until all are paired (or one is left alone if odd)
 * 4. Non-deterministic outcome due to random student selection
 * 
 * Message types used:
 * - TOKEN: "Your turn to pick a partner" (carries count of unpaired)
 * - PROPOSE: "Will you partner with me?"
 * - ACCEPT: "Yes, I accept!"
 * - STATUS: "Are you still unpaired?"
 * - DONE: Broadcast completion signal
 */
 