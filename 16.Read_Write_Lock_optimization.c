#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/* Your output confirms that the writer prioritization mechanism is now functioning correctly:

   Multiple readers start together when no writers are waiting.
   After several reads, a writer is prioritized, preventing writer starvation.
   Writers execute one at a time, ensuring fairness.
   Once all writers finish, new readers can start again.
*/   

// Read-Write Lock structure with writer prioritization
typedef struct {
    int readers;          // Number of active readers
    int writers;          // Number of active writers (should be 0 or 1)
    int writeRequests;    // Number of writers waiting
    int readCount;        // Number of reads completed before allowing a writer
    pthread_mutex_t mutex;
    pthread_cond_t readCondition;
    pthread_cond_t writeCondition;
} ReadWriteLock;

#define READ_LIMIT 5  // Limit number of continuous reads before a writer gets priority

// Get the current timestamp for debugging purposes
void printTimestamp(const char* msg, pthread_t threadId) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%ld.%ld] %s - Thread %lu\n", ts.tv_sec, ts.tv_nsec / 1000000, msg, threadId);
}

// Initialize the Read-Write Lock
void initReadWriteLock(ReadWriteLock* lock) {
    lock->readers = 0;
    lock->writers = 0;
    lock->writeRequests = 0;
    lock->readCount = 0;
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readCondition, NULL);
    pthread_cond_init(&lock->writeCondition, NULL);
}

// Acquire read lock (with writer prioritization)
void lockRead(ReadWriteLock* lock) {
    pthread_mutex_lock(&lock->mutex);
    
    // If there are write requests, wait to allow writers priority
    while (lock->writers > 0 || (lock->writeRequests > 0 && lock->readCount >= READ_LIMIT)) {
        pthread_cond_wait(&lock->readCondition, &lock->mutex);
    }
    
    lock->readers++;
    lock->readCount++;
    
    pthread_mutex_unlock(&lock->mutex);
    printTimestamp("Reader started", pthread_self());
}

// Release read lock
void unlockRead(ReadWriteLock* lock) {
    pthread_mutex_lock(&lock->mutex);
    
    lock->readers--;
    
    // Reset read count if no readers are active
    if (lock->readers == 0) {
        lock->readCount = 0;
        pthread_cond_signal(&lock->writeCondition); // Signal waiting writer if any
    }
    
    pthread_mutex_unlock(&lock->mutex);
    printTimestamp("Reader finished", pthread_self());
}

// Acquire write lock (writer prioritization included)
void lockWrite(ReadWriteLock* lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->writeRequests++;
    
    // Writers wait if there are active readers or another writer
    while (lock->readers > 0 || lock->writers > 0) {
        pthread_cond_wait(&lock->writeCondition, &lock->mutex);
    }
    
    lock->writeRequests--;
    lock->writers++;
    pthread_mutex_unlock(&lock->mutex);
    printTimestamp("Writer started", pthread_self());
}

// Release write lock
void unlockWrite(ReadWriteLock* lock) {
    pthread_mutex_lock(&lock->mutex);
    
    lock->writers--;
    
    // Prioritize writers if any are waiting, otherwise wake up readers
    if (lock->writeRequests > 0) {
        pthread_cond_signal(&lock->writeCondition);
    } else {
        pthread_cond_broadcast(&lock->readCondition);
    }
    
    pthread_mutex_unlock(&lock->mutex);
    printTimestamp("Writer finished", pthread_self());
}

// Example usage - Reader thread function
void* reader(void* arg) {
    ReadWriteLock* lock = (ReadWriteLock*)arg;
    lockRead(lock);
    sleep(1); // Simulate reading
    unlockRead(lock);
    return NULL;
}

// Example usage - Writer thread function
void* writer(void* arg) {
    ReadWriteLock* lock = (ReadWriteLock*)arg;
    lockWrite(lock);
    sleep(2); // Simulate writing
    unlockWrite(lock);
    return NULL;
}

int main() {
    ReadWriteLock lock;
    initReadWriteLock(&lock);
    
    pthread_t readers[5], writers[2];
    
    // Create reader threads
    for (int i = 0; i < 5; i++) {
        pthread_create(&readers[i], NULL, reader, &lock);
    }
    
    // Create writer threads
    for (int i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer, &lock);
    }
    
    // Join reader threads
    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }
    
    // Join writer threads
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }
    
    return 0;
}
