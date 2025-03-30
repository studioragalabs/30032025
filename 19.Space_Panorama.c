#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* Key Features Implemented

   Row-Based Hash Table for Efficient Sector Lookups
   - Uses row-major hashing (sector.x % MAX_CACHE_SIZE) to minimize lookup overhead.
   - Ensures efficient retrieval of images with O(1) average access time.

   LRU Cache for Memory-Efficient Storage
   - Implements Least Recently Used (LRU) eviction using a doubly linked list.
   - Removes oldest images when cache reaches max capacity.

   Multi-threading Support for Concurrent Access
   - Uses mutex locks (pthread_mutex_t) to prevent race conditions.
   - Allows multiple telescope images to be added/retrieved concurrently.
   
   Minimal Memory Overhead
   - Only stores valid images within the cache limit (MAX_CACHE_SIZE).
   - Prevents unbounded memory growth.

   Now, the Space Panorama system can efficiently store and retrieve sector images from deep space!

   Next Step:
   Further optimizations, such as distributed caching across multiple servers?

*/

#define MAX_CACHE_SIZE 100 // Maximum cache size for LRU eviction

// Structure for representing an image
typedef struct {
    char *data;
    size_t size;
} Image;

// Structure for representing a sector
typedef struct {
    int x, y;
} Sector;

// Structure for a doubly linked list node (for LRU Cache)
typedef struct DLinkedList {
    Sector key;
    Image value;
    struct DLinkedList *prev, *next;
} DLinkedList;

// Structure for the LRU Cache
typedef struct {
    int capacity, count;
    DLinkedList *head, *tail;
    pthread_mutex_t lock;
} LRUCache;

// Hash table structure for row-wise sector storage
typedef struct {
    DLinkedList *table[MAX_CACHE_SIZE];
} RowHashTable;

RowHashTable rowHash;

// Function prototypes
DLinkedList* createNode(Sector key, Image value);
void removeNode(LRUCache *cache, DLinkedList *node);
void moveToTail(LRUCache *cache, DLinkedList *node);
Image* getImage(LRUCache *cache, Sector key);
void putImage(LRUCache *cache, Sector key, Image value);
void removeLRU(LRUCache *cache);

// Initialize LRU Cache
void initLRUCache(LRUCache *cache, int capacity) {
    cache->capacity = capacity;
    cache->count = 0;
    cache->head = (DLinkedList*)malloc(sizeof(DLinkedList));
    cache->tail = (DLinkedList*)malloc(sizeof(DLinkedList));
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    pthread_mutex_init(&cache->lock, NULL);
}

// Create a new node for LRU Cache
DLinkedList* createNode(Sector key, Image value) {
    DLinkedList *node = (DLinkedList*)malloc(sizeof(DLinkedList));
    node->key = key;
    node->value = value;
    node->prev = node->next = NULL;
    return node;
}

// Remove a node from the linked list
void removeNode(LRUCache *cache, DLinkedList *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

// Move node to the tail (most recently used)
void moveToTail(LRUCache *cache, DLinkedList *node) {
    node->prev = cache->tail->prev;
    cache->tail->prev->next = node;
    node->next = cache->tail;
    cache->tail->prev = node;
}

// Retrieve an image from the LRU Cache
Image* getImage(LRUCache *cache, Sector key) {
    pthread_mutex_lock(&cache->lock);
    int index = key.x % MAX_CACHE_SIZE; // Hash row index
    DLinkedList *node = rowHash.table[index];
    
    while (node) {
        if (node->key.x == key.x && node->key.y == key.y) {
            removeNode(cache, node);
            moveToTail(cache, node);
            pthread_mutex_unlock(&cache->lock);
            return &node->value;
        }
        node = node->next;
    }
    pthread_mutex_unlock(&cache->lock);
    return NULL;
}

// Store an image in the LRU Cache
void putImage(LRUCache *cache, Sector key, Image value) {
    pthread_mutex_lock(&cache->lock);
    
    int index = key.x % MAX_CACHE_SIZE; // Hash row index
    DLinkedList *node = rowHash.table[index];
    
    while (node) {
        if (node->key.x == key.x && node->key.y == key.y) {
            node->value = value;
            removeNode(cache, node);
            moveToTail(cache, node);
            pthread_mutex_unlock(&cache->lock);
            return;
        }
        node = node->next;
    }
    
    if (cache->count == cache->capacity) {
        removeLRU(cache);
    }
    
    node = createNode(key, value);
    moveToTail(cache, node);
    rowHash.table[index] = node;
    cache->count++;
    
    pthread_mutex_unlock(&cache->lock);
}

// Remove the least recently used item
void removeLRU(LRUCache *cache) {
    DLinkedList *lru = cache->head->next;
    removeNode(cache, lru);
    int index = lru->key.x % MAX_CACHE_SIZE;
    rowHash.table[index] = NULL;
    free(lru);
    cache->count--;
}

// Main function to test the Space Panorama system
int main() {
    LRUCache cache;
    initLRUCache(&cache, MAX_CACHE_SIZE);
    
    Sector sector1 = {0, 0};
    Sector sector2 = {1, 2};
    Image img1 = {"ImageData1", 10};
    Image img2 = {"ImageData2", 10};
    
    putImage(&cache, sector1, img1);
    putImage(&cache, sector2, img2);
    
    Image *retrievedImg = getImage(&cache, sector2);
    if (retrievedImg) {
        printf("Retrieved Image: %s\n", retrievedImg->data);
    } else {
        printf("Image not found!\n");
    }
    
    return 0;
}

