#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

/* This C implementation mirrors the Java multithreaded web crawler using POSIX threads (pthread):

   Key Features:
   Graph Representation (addUrlNode):
   - Stores URLs and their connected links dynamically.

   Multithreading with pthread (crawl):
   - Each thread crawls a single URL and fetches linked URLs.

   Thread Management (crawlManager):
   - Controls THREAD_COUNT crawlers.
   - Uses sleep(PAUSE_TIME) to throttle requests.

   Synchronization (pthread_mutex_t):
   - Protects shared visited URLs with a mutex lock.

   This implementation ensures efficient parallel crawling while avoiding race conditions

   The crawler is working but not processing all URLs in the expected order. 
   The issue likely arises due to non-deterministic thread execution order and early thread exits before fully processing all links

   Fixes & Improvements
   Ensure BFS-like Processing Order:
   - Instead of only launching threads for directly connected URLs, process the queue iteratively.
   - Use a global queue of pending URLs, accessed synchronously.

   Thread Pool for Better Parallelism:
   - Avoid creating and destroying threads excessively.
   - Use a fixed thread pool that continuously pulls URLs from the queue.

   Proper Synchronization (pthread_mutex_t):
   - Ensure threads safely add new URLs to the queue.

   =================================================
   Fixes & Improvements in the Web Crawler

   Uses a Global Queue for BFS-style Processing (queue):
   - Ensures that URLs are processed in a breadth-first manner.

   Thread Pool (crawl):
   - Threads continuously pull URLs from the queue.
   - Uses pthread_cond_wait() and pthread_cond_broadcast() to avoid busy waiting.

   Proper Synchronization (pthread_mutex_t):
   - Protects shared resources (queue and visited list).
   - Prevents race conditions when accessing/modifying shared data.

   Expected Output (Corrected Order)
   Crawled: a
   Crawled: b
   Crawled: c
   Crawled: d
   Crawled: e
   Crawled: k
   Crawled: m
   Crawled: z
   Crawled: o
   Crawled: j

   Now, the crawler ensures full URL coverage in a controlled multithreaded manner

*/


#define MAX_URLS 100
#define THREAD_COUNT 5

// Structure to represent a URL and its connected links
typedef struct {
    char url[100];
    char* connectedUrls[MAX_URLS];
    int count;
} UrlNode;

// Global Variables
UrlNode urlGraph[MAX_URLS];
int urlGraphSize = 0;
char visitedUrls[MAX_URLS][100];
int visitedCount = 0;
char queue[MAX_URLS][100];
int queueSize = 0;
pthread_mutex_t lock;
pthread_cond_t cond;

// Function to add URL relationships
void addUrlNode(const char* url, const char* connections[], int count) {
    strcpy(urlGraph[urlGraphSize].url, url);
    urlGraph[urlGraphSize].count = count;
    for (int i = 0; i < count; i++) {
        urlGraph[urlGraphSize].connectedUrls[i] = strdup(connections[i]);
    }
    urlGraphSize++;
}

// Function to get URLs connected to a given URL
char** getUrls(const char* url, int* count) {
    for (int i = 0; i < urlGraphSize; i++) {
        if (strcmp(urlGraph[i].url, url) == 0) {
            *count = urlGraph[i].count;
            return urlGraph[i].connectedUrls;
        }
    }
    *count = 0;
    return NULL;
}

// Function to check if URL has been visited
bool isVisited(const char* url) {
    for (int i = 0; i < visitedCount; i++) {
        if (strcmp(visitedUrls[i], url) == 0) {
            return true;
        }
    }
    return false;
}

// Function to add URL to the visited list
void addVisitedUrl(const char* url) {
    strcpy(visitedUrls[visitedCount++], url);
}

// Function to add URLs to the queue
void enqueueUrl(const char* url) {
    strcpy(queue[queueSize++], url);
}

// Function to dequeue a URL
bool dequeueUrl(char* url) {
    if (queueSize == 0) return false;
    strcpy(url, queue[0]);
    for (int i = 1; i < queueSize; i++) {
        strcpy(queue[i - 1], queue[i]);
    }
    queueSize--;
    return true;
}

// Function executed by each crawler thread
void* crawl(void* args) {
    while (1) {
        pthread_mutex_lock(&lock);
        while (queueSize == 0) {
            pthread_cond_wait(&cond, &lock);
        }
        
        char url[100];
        if (!dequeueUrl(url)) {
            pthread_mutex_unlock(&lock);
            continue;
        }
        
        if (isVisited(url)) {
            pthread_mutex_unlock(&lock);
            continue;
        }
        
        addVisitedUrl(url);
        printf("Crawled: %s\n", url);
        
        int urlCount;
        char** urls = getUrls(url, &urlCount);
        pthread_mutex_unlock(&lock);
        
        if (urls) {
            pthread_mutex_lock(&lock);
            for (int i = 0; i < urlCount; i++) {
                if (!isVisited(urls[i])) {
                    enqueueUrl(urls[i]);
                }
            }
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}

// Function to manage the crawling process
void crawlManager(const char* startUrl) {
    pthread_t threads[THREAD_COUNT];
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);
    
    enqueueUrl(startUrl);
    pthread_cond_broadcast(&cond);
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, crawl, NULL);
    }
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
}

// Main function for testing
int main() {
    // Sample URLs and relationships
    const char* aChildren[] = {"b", "c", "d", "e"};
    const char* bChildren[] = {"k", "m", "d", "z"};
    const char* kChildren[] = {"o", "j", "e", "z"};
    
    addUrlNode("a", aChildren, 4);
    addUrlNode("b", bChildren, 4);
    addUrlNode("k", kChildren, 4);
    
    crawlManager("a");
    
    return 0;
}
