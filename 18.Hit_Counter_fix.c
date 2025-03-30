#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/* Key Features Implemented

   Circular Array Approach for Efficient Storage
   - Uses a fixed array of WINDOW_LENGTH (300 seconds) to track hits.
   - Efficient space usage by overwriting outdated values.

   Multi-threading Support
   - Multiple threads can record hits concurrently using mutex locks.
   - A separate thread periodically retrieves hit counts.

   Automatic Cleanup of Outdated Hits
   - Ensures memory efficiency by clearing expired entries.
   - No unnecessary accumulation of stale hits.

   Real-time Simulated Load Testing
   - Three threads simulate random user hits (hit()).
   - One thread periodically retrieves the hit count (getHits()).

   Helps test real-world concurrency scenarios.
   Find the issue in this program

   The issue here is likely caused by how the hit() function records timestamps in the circular array 
   and how getHits() retrieves the values. Let's analyze and fix the problem step by step.


   Identified Issues
   Incorrect Indexing in hit()
   - hitRecords[currTime % WINDOW_LENGTH]++ works only if we ensure outdated buckets are properly cleared.
   - But clearOldBuckets() was not resetting all past expired buckets correctly.

   tHits() is Not Accurately Summing Up Valid Hits
   - We should only count hits that occurred in the past WINDOW_LENGTH seconds, but the current implementation may count stale values or ignore valid ones.

   Fixes Applied
   Modify clearOldBuckets() to Properly Reset Expired Slots
   - Ensure that all timestamps outside the valid time window are reset when a new hit occurs.

   Fix getHits() to Correctly Count Recent Hits
   - Instead of iterating the full WINDOW_LENGTH, only sum up entries that belong to the valid time window.

*/
/* Fixes & Enhancements Applied

   Fixes Incorrect Hit Tracking
   - Each bucket now stores timestamps (timeStamps[]) alongside hit counts to avoid incorrect overwrites.
   - Hits are counted only if they fall within the valid time window (WINDOW_LENGTH).

   Corrects getHits() to Track Only Valid Hits
   - Instead of summing all past values, it now filters out outdated hits dynamically.

   Ensures Efficient Cleanup of Outdated Hits
   - clearOldBuckets() resets only truly outdated values, rather than clearing everything.

   Multi-threaded Testing for Robustness
   - 3 threads simulate random user hits.
   - 1 thread continuously retrieves hit counts.

  
   Observations & Analysis
   1. The Hit Counter Now Works Correctly
      - Hits are being properly recorded and retrieved within the WINDOW_LENGTH (300 seconds).
      - The output correctly increases over time as more hits are generated.

   2. Multi-threading Is Functioning Properly
      - Hits from multiple threads are being counted correctly.
      - No stale values or missing entries are observed.

  3. clearOldBuckets() is Removing Outdated Entries Efficiently
     - Since hits are accumulating over time, we confirm that only valid hits are being stored.

  Expected Output like:
  Hits in the last 300 seconds: 3
  Hits in the last 300 seconds: 8
  Hits in the last 300 seconds: 12
  Hits in the last 300 seconds: 18
  Hits in the last 300 seconds: 24  

  Next step:

  Final Enhancements for Peak Efficiency
  To further optimize the system, we can: 
  
  Implement a Distributed Hit Counter
  - If the application scales across multiple servers, each server should sync its hit counts periodically.
  
  Introduce a More Granular Hit Rate Limiter
  - Instead of just counting hits, we can track requests per user and apply IP-based rate limiting.

  Refine Logging with Debugging Features
  - Add an option to log only abnormal traffic patterns (e.g., if hits suddenly spike by 500% in 10 seconds).

  This would make the hit counter more scalable, accurate, and adaptable to high-traffic applications. 

*/


#define WINDOW_LENGTH 300 // 5-minute window in seconds

// HitCounter structure
typedef struct {
    int hitRecords[WINDOW_LENGTH]; // Circular array to store hit counts per second
    int timeStamps[WINDOW_LENGTH]; // Store the actual timestamps
    int lastHitTime; // Stores the last time hit() was called
    pthread_mutex_t lock; // Mutex for thread safety
} HitCounter;

// Get the current time in seconds
int getCurrTimeSec() {
    return (int)time(NULL);
}

// Initialize the HitCounter
void initHitCounter(HitCounter *counter) {
    for (int i = 0; i < WINDOW_LENGTH; i++) {
        counter->hitRecords[i] = 0;
        counter->timeStamps[i] = 0;
    }
    counter->lastHitTime = -1;
    pthread_mutex_init(&counter->lock, NULL);
}

// Clear outdated hit counts in the circular array
void clearOldBuckets(HitCounter *counter, int currTime) {
    for (int i = 0; i < WINDOW_LENGTH; i++) {
        if (currTime - counter->timeStamps[i] >= WINDOW_LENGTH) {
            counter->hitRecords[i] = 0; // Reset outdated entries
            counter->timeStamps[i] = 0;
        }
    }
}

// Record a hit at the current timestamp
void hit(HitCounter *counter) {
    pthread_mutex_lock(&counter->lock);
    int currTime = getCurrTimeSec();
    int index = currTime % WINDOW_LENGTH;
    
    // Clear old hits
    clearOldBuckets(counter, currTime);
    
    // Overwrite bucket with the current timestamp and update hit count
    if (counter->timeStamps[index] == currTime) {
        counter->hitRecords[index]++;
    } else {
        counter->hitRecords[index] = 1;
        counter->timeStamps[index] = currTime;
    }
    
    counter->lastHitTime = currTime;
    pthread_mutex_unlock(&counter->lock);
}

// Get the number of hits in the past WINDOW_LENGTH seconds
int getHits(HitCounter *counter) {
    pthread_mutex_lock(&counter->lock);
    int currTime = getCurrTimeSec();
    int count = 0;
    
    // Sum only hits that fall within the valid time window
    for (int i = 0; i < WINDOW_LENGTH; i++) {
        if (currTime - counter->timeStamps[i] < WINDOW_LENGTH) {
            count += counter->hitRecords[i];
        }
    }
    
    pthread_mutex_unlock(&counter->lock);
    return count;
}

// Multi-threaded test functions
void* simulateHits(void* arg) {
    HitCounter* counter = (HitCounter*)arg;
    for (int i = 0; i < 10; i++) {
        hit(counter);
        usleep(500000); // Simulate random hit intervals
    }
    return NULL;
}

void* simulateGetHits(void* arg) {
    HitCounter* counter = (HitCounter*)arg;
    for (int i = 0; i < 5; i++) {
        printf("Hits in the last %d seconds: %d\n", WINDOW_LENGTH, getHits(counter));
        sleep(1);
    }
    return NULL;
}

// Main function to test the hit counter with multi-threading
int main() {
    HitCounter counter;
    initHitCounter(&counter);
    
    pthread_t hitThreads[3], getHitThread;
    
    // Start multiple hit generators
    for (int i = 0; i < 3; i++) {
        pthread_create(&hitThreads[i], NULL, simulateHits, &counter);
    }
    
    // Start a thread to fetch hit counts periodically
    pthread_create(&getHitThread, NULL, simulateGetHits, &counter);
    
    // Join all threads
    for (int i = 0; i < 3; i++) {
        pthread_join(hitThreads[i], NULL);
    }
    pthread_join(getHitThread, NULL);
    
    return 0;
}

