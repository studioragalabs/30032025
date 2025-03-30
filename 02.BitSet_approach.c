#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

#define MAX_ID 100  // Adjust as needed

// ---------------------------- FreeList Approach ----------------------------
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

// ---------------------------- BitSet Approach ----------------------------
typedef struct {
    unsigned char *bitSet; // Using an array of bytes as a bit array
    int maxId;
    int nextAvailable;
} BitSetAllocator;

// Function to create a BitSetAllocator
BitSetAllocator* createBitSetAllocator(int maxId) {
    BitSetAllocator *allocator = (BitSetAllocator*)malloc(sizeof(BitSetAllocator));
    allocator->maxId = maxId;
    allocator->bitSet = (unsigned char*)calloc((maxId + 7) / 8, sizeof(unsigned char));
    allocator->nextAvailable = 0;
    return allocator;
}

// Function to check if a bit is set
bool isBitSet(BitSetAllocator *allocator, int id) {
    return allocator->bitSet[id / 8] & (1 << (id % 8));
}

// Function to set a bit
void setBit(BitSetAllocator *allocator, int id) {
    allocator->bitSet[id / 8] |= (1 << (id % 8));
}

// Function to clear a bit
void clearBit(BitSetAllocator *allocator, int id) {
    allocator->bitSet[id / 8] &= ~(1 << (id % 8));
}

// Function to allocate an ID
int allocateBitSet(BitSetAllocator *allocator) {
    if (allocator->nextAvailable == allocator->maxId) return -1; // No available ID
    int id = allocator->nextAvailable;
    setBit(allocator, id);
    while (allocator->nextAvailable < allocator->maxId && isBitSet(allocator, allocator->nextAvailable)) {
        allocator->nextAvailable++;
    }
    return id;
}

// Function to release an ID
void releaseBitSet(BitSetAllocator *allocator, int id) {
    if (id < 0 || id >= allocator->maxId) return;
    if (isBitSet(allocator, id)) {
        clearBit(allocator, id);
        if (id < allocator->nextAvailable) {
            allocator->nextAvailable = id;
        }
    }
}

// Function to check if an ID is available
bool checkBitSet(BitSetAllocator *allocator, int id) {
    if (id < 0 || id >= allocator->maxId) return false;
    return !isBitSet(allocator, id);
}

// Function to free memory
void destroyBitSetAllocator(BitSetAllocator *allocator) {
    free(allocator->bitSet);
    free(allocator);
}

// ---------------------------- Main Function ----------------------------
int main() {
    printf("Testing FreeList Allocator:\n");
    Allocator *allocator = createAllocator(10);
    int id1 = allocate(allocator);
    int id2 = allocate(allocator);
    int id3 = allocate(allocator);
    printf("Allocated IDs: %d, %d, %d\n", id1, id2, id3);
    release(allocator, id2);
    printf("After release, check ID %d: %s\n", id2, check(allocator, id2) ? "Available" : "Not Available");
    destroyAllocator(allocator);
    
    printf("\nTesting BitSet Allocator:\n");
    BitSetAllocator *bitAllocator = createBitSetAllocator(10);
    int b1 = allocateBitSet(bitAllocator);
    int b2 = allocateBitSet(bitAllocator);
    int b3 = allocateBitSet(bitAllocator);
    printf("Allocated IDs: %d, %d, %d\n", b1, b2, b3);
    releaseBitSet(bitAllocator, b2);
    printf("After release, check ID %d: %s\n", b2, checkBitSet(bitAllocator, b2) ? "Available" : "Not Available");
    destroyBitSetAllocator(bitAllocator);
    return 0;
}

