#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PHOTOS 10000

// Doubly linked list node structure
typedef struct DListNode {
    int freq;
    struct DListNode* prev;
    struct DListNode* next;
    struct PhotoIDNode* photoHead;
} DListNode;

// Photo ID node in a linked list
typedef struct PhotoIDNode {
    int id;
    struct PhotoIDNode* next;
} PhotoIDNode;

// Hash map entry for tracking frequency
typedef struct {
    int id;
    int freq;
    DListNode* bucket;
} HashEntry;

// Frequency to bucket hash map
typedef struct {
    DListNode* buckets[MAX_PHOTOS];
} FreqMap;

// Structure to track top K photo views
typedef struct {
    HashEntry entries[MAX_PHOTOS];
    FreqMap freqMap;
    DListNode* freqHead;
    int k;
} TopKPhoto;

// Create a new doubly linked list node
DListNode* createDListNode(int freq) {
    DListNode* node = (DListNode*)malloc(sizeof(DListNode));
    node->freq = freq;
    node->prev = node->next = NULL;
    node->photoHead = NULL;
    return node;
}

// Create a new photo ID node
PhotoIDNode* createPhotoIDNode(int id) {
    PhotoIDNode* node = (PhotoIDNode*)malloc(sizeof(PhotoIDNode));
    node->id = id;
    node->next = NULL;
    return node;
}

// Initialize the top K structure
void initTopK(TopKPhoto* tracker, int k) {
    memset(tracker->entries, 0, sizeof(tracker->entries));
    memset(tracker->freqMap.buckets, 0, sizeof(tracker->freqMap.buckets));
    tracker->freqHead = NULL;
    tracker->k = k;
}

// Update the view count for a photo ID using a frequency hash map
void viewPhoto(TopKPhoto* tracker, int id) {
    if (id < 0 || id >= MAX_PHOTOS) return;
    
    HashEntry* entry = &tracker->entries[id];
    if (entry->id == 0) {
        entry->id = id;
        entry->freq = 1;
        
        if (!tracker->freqMap.buckets[1]) {
            DListNode* newNode = createDListNode(1);
            newNode->next = tracker->freqHead;
            if (tracker->freqHead) tracker->freqHead->prev = newNode;
            tracker->freqHead = newNode;
            tracker->freqMap.buckets[1] = newNode;
        }
        entry->bucket = tracker->freqMap.buckets[1];
        
        PhotoIDNode* newPhoto = createPhotoIDNode(id);
        newPhoto->next = entry->bucket->photoHead;
        entry->bucket->photoHead = newPhoto;
    } else {
        int newFreq = entry->freq + 1;
        DListNode* currentBucket = entry->bucket;
        
        if (!tracker->freqMap.buckets[newFreq]) {
            DListNode* newNode = createDListNode(newFreq);
            newNode->next = currentBucket->next;
            if (currentBucket->next) {
                currentBucket->next->prev = newNode;
            }
            currentBucket->next = newNode;
            newNode->prev = currentBucket;
            tracker->freqMap.buckets[newFreq] = newNode;
        }
        
        entry->freq = newFreq;
        entry->bucket = tracker->freqMap.buckets[newFreq];
    }
}

// Print the top K viewed photos efficiently
void printTopK(TopKPhoto* tracker) {
    printf("Top %d most viewed photos:\n", tracker->k);
    DListNode* current = tracker->freqHead;
    int count = 0;
    
    while (current && count < tracker->k) {
        if (current->photoHead) {
            PhotoIDNode* photoNode = current->photoHead;
            while (photoNode && count < tracker->k) {
                printf("Photo ID: %d, Views: %d\n", photoNode->id, current->freq);
                photoNode = photoNode->next;
                count++;
            }
        }
        current = current->next;
    }
}

int main() {
    TopKPhoto tracker;
    initTopK(&tracker, 4);
    
    int photoIds[] = {1, 2, 1, 3, 2, 12, 31, 101, 11, 3, 31, 101, 31, 101};
    int size = sizeof(photoIds) / sizeof(photoIds[0]);
    
    for (int i = 0; i < size; i++) {
        viewPhoto(&tracker, photoIds[i]);
    }
    
    printTopK(&tracker);
    return 0;
}

