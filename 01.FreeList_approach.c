#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_ID 100  // Adjust as needed

// Structure to define the Allocator using FreeList Approach
typedef struct {
    int *freeList;  // Queue (Array-based)
    bool *allocated;  // Set (Boolean array)
    int maxId;
    int front, rear, size;
} Allocator;

// Function to initialize the allocator
Allocator* createAllocator(int maxId) {
    Allocator *allocator = (Allocator*)malloc(sizeof(Allocator));
    allocator->maxId = maxId;
    allocator->freeList = (int*)malloc(maxId * sizeof(int));
    allocator->allocated = (bool*)calloc(maxId, sizeof(bool)); // Default all false
    allocator->front = 0;
    allocator->rear = maxId - 1;
    allocator->size = maxId;
    
    for (int i = 0; i < maxId; i++) {
        allocator->freeList[i] = i;
    }
    return allocator;
}

// Function to allocate an ID
int allocate(Allocator *allocator) {
    if (allocator->size == 0) return -1; // No available ID
    
    int id = allocator->freeList[allocator->front];
    allocator->allocated[id] = true;
    allocator->front = (allocator->front + 1) % allocator->maxId;
    allocator->size--;
    return id;
}

// Function to release an ID
void release(Allocator *allocator, int id) {
    if (id < 0 || id >= allocator->maxId || !allocator->allocated[id]) return;
    
    allocator->allocated[id] = false;
    allocator->rear = (allocator->rear + 1) % allocator->maxId;
    allocator->freeList[allocator->rear] = id;
    allocator->size++;
}

// Function to check if an ID is available
bool check(Allocator *allocator, int id) {
    if (id < 0 || id >= allocator->maxId) return false;
    return !allocator->allocated[id];
}

// Function to free memory
void destroyAllocator(Allocator *allocator) {
    free(allocator->freeList);
    free(allocator->allocated);
    free(allocator);
}

// Main function to test the allocator
int main() {
    Allocator *allocator = createAllocator(10);
    
    int id1 = allocate(allocator);
    int id2 = allocate(allocator);
    int id3 = allocate(allocator);
    printf("Allocated IDs: %d, %d, %d\n", id1, id2, id3);
    
    printf("Check ID %d: %s\n", id1, check(allocator, id1) ? "Available" : "Not Available");
    printf("Check ID %d: %s\n", id3, check(allocator, id3) ? "Available" : "Not Available");
    
    release(allocator, id2);
    printf("After release, check ID %d: %s\n", id2, check(allocator, id2) ? "Available" : "Not Available");
    
    destroyAllocator(allocator);
    return 0;
}

