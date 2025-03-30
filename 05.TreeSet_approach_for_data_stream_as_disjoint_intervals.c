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

// ---------------------------- TreeSet Approach for Data Stream as Disjoint Intervals ----------------------------
typedef struct Interval {
    int start, end;
    struct Interval *next;
} Interval;

typedef struct {
    Interval *head;
} SummaryRanges;

// Function to create a SummaryRanges object
SummaryRanges* createSummaryRanges() {
    SummaryRanges *sr = (SummaryRanges*)malloc(sizeof(SummaryRanges));
    sr->head = NULL;
    return sr;
}

// Function to add a number to the data stream
void addNum(SummaryRanges *sr, int val) {
    Interval *newInterval = (Interval*)malloc(sizeof(Interval));
    newInterval->start = val;
    newInterval->end = val;
    newInterval->next = NULL;
    
    if (!sr->head) {
        sr->head = newInterval;
        return;
    }
    
    Interval *prev = NULL, *current = sr->head;
    while (current) {
        if (val < current->start - 1) {
            newInterval->next = current;
            if (prev) prev->next = newInterval;
            else sr->head = newInterval;
            return;
        } else if (val == current->start - 1) {
            current->start = val;
            return;
        } else if (val >= current->start && val <= current->end) {
            return; // Already covered
        } else if (val == current->end + 1) {
            current->end = val;
            if (current->next && current->next->start == val + 1) {
                current->end = current->next->end;
                Interval *temp = current->next;
                current->next = temp->next;
                free(temp);
            }
            return;
        }
        prev = current;
        current = current->next;
    }
    
    prev->next = newInterval;
}

// Function to print the intervals
void printIntervals(SummaryRanges *sr) {
    Interval *current = sr->head;
    printf("Intervals: ");
    while (current) {
        printf("[%d, %d] ", current->start, current->end);
        current = current->next;
    }
    printf("\n");
}

// Function to free memory
void destroySummaryRanges(SummaryRanges *sr) {
    Interval *current = sr->head;
    while (current) {
        Interval *temp = current;
        current = current->next;
        free(temp);
    }
    free(sr);
}

// ---------------------------- Main Function ----------------------------
int main() {
    // Testing TreeSet Approach for Data Stream as Disjoint Intervals
    SummaryRanges *sr = createSummaryRanges();
    addNum(sr, 1);
    addNum(sr, 3);
    addNum(sr, 7);
    addNum(sr, 2);
    addNum(sr, 6);
    printIntervals(sr);
    destroySummaryRanges(sr);
    return 0;
}

