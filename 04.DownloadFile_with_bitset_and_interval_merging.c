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

// ---------------------------- Download File with BitSet ----------------------------
typedef struct {
    unsigned char *bitSet; // BitSet to track downloaded chunks
    int size;
} DownloaderBitSet;

// Function to create a DownloaderBitSet
DownloaderBitSet* createDownloader(int size) {
    DownloaderBitSet *downloader = (DownloaderBitSet*)malloc(sizeof(DownloaderBitSet));
    downloader->size = size;
    downloader->bitSet = (unsigned char*)calloc((size + 7) / 8, sizeof(unsigned char));
    return downloader;
}

// Function to set a bit (mark chunk as downloaded)
void setDownloadBit(DownloaderBitSet *downloader, int start, int end) {
    for (int i = start; i < end; i++) {
        downloader->bitSet[i / 8] |= (1 << (i % 8));
    }
}

// Function to check if file is fully downloaded
bool isDownloadComplete(DownloaderBitSet *downloader) {
    for (int i = 0; i < downloader->size; i++) {
        if (!(downloader->bitSet[i / 8] & (1 << (i % 8)))) {
            return false;
        }
    }
    return true;
}

// Function to free memory
void destroyDownloader(DownloaderBitSet *downloader) {
    free(downloader->bitSet);
    free(downloader);
}

// ---------------------------- Interval Merging Approach ----------------------------
typedef struct Interval {
    int start, end;
    struct Interval *next;
} Interval;

typedef struct {
    Interval *head;
    int size;
} DownloaderInterval;

// Function to create a Downloader using Interval Merging
DownloaderInterval* createDownloaderInterval(int size) {
    DownloaderInterval *downloader = (DownloaderInterval*)malloc(sizeof(DownloaderInterval));
    downloader->head = NULL;
    downloader->size = size;
    return downloader;
}

// Function to add a downloaded chunk
void addDownloadChunk(DownloaderInterval *downloader, int start, int end) {
    Interval *newChunk = (Interval*)malloc(sizeof(Interval));
    newChunk->start = start;
    newChunk->end = end;
    newChunk->next = NULL;
    
    if (!downloader->head || downloader->head->start > end) {
        newChunk->next = downloader->head;
        downloader->head = newChunk;
        return;
    }
    
    Interval *prev = NULL, *current = downloader->head;
    while (current && current->start <= end) {
        if (current->end >= start) {
            if (current->start > start) current->start = start;
            if (current->end < end) current->end = end;
            return;
        }
        prev = current;
        current = current->next;
    }
    
    newChunk->next = current;
    if (prev) prev->next = newChunk;
}

// Function to check if the file is fully downloaded
bool isDownloadCompleteInterval(DownloaderInterval *downloader) {
    return downloader->head && downloader->head->start == 0 && downloader->head->end == downloader->size;
}

// Function to free memory
void destroyDownloaderInterval(DownloaderInterval *downloader) {
    Interval *current = downloader->head;
    while (current) {
        Interval *temp = current;
        current = current->next;
        free(temp);
    }
    free(downloader);
}

// ---------------------------- Main Function ----------------------------
int main() {
    // Testing Download using BitSet
    DownloaderBitSet *bitDownloader = createDownloader(10);
    setDownloadBit(bitDownloader, 0, 5);
    setDownloadBit(bitDownloader, 5, 10);
    printf("File downloaded (BitSet): %s\n", isDownloadComplete(bitDownloader) ? "Yes" : "No");
    destroyDownloader(bitDownloader);
    
    // Testing Download using Interval Merging
    DownloaderInterval *intervalDownloader = createDownloaderInterval(10);
    addDownloadChunk(intervalDownloader, 0, 5);
    addDownloadChunk(intervalDownloader, 5, 10);
    printf("File downloaded (Interval Merging): %s\n", isDownloadCompleteInterval(intervalDownloader) ? "Yes" : "No");
    destroyDownloaderInterval(intervalDownloader);
    
    return 0;
}

