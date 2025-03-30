#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LARGE_PRIME 105613

// Simple substring match
int contains_bytes(const unsigned char* pattern, int pattern_len, const unsigned char* text, int text_len) {
    int count = 0;
    for (int i = 0; i <= text_len - pattern_len; i++) {
        int j = 0;
        while (j < pattern_len && pattern[j] == text[i + j]) {
            j++;
        }
        if (j == pattern_len) {
            printf("Pattern found at position %d\n", i);
            count++;
        }
    }
    printf("Total occurrences: %d\n", count);
    return count > 0;
}

// Rolling Hash Structure
typedef struct {
    int a;
    int h;
    int window_length;
    long curr_hash_value;
} RollingHash;

// Initialize Rolling Hash
void init_rolling_hash(RollingHash *hashFun, int a, const unsigned char* initialBytes, int len) {
    hashFun->a = a;
    hashFun->window_length = len;
    hashFun->h = 1;
    
    for (int i = 0; i < len - 1; i++) {
        hashFun->h = (hashFun->h * a) % LARGE_PRIME;
    }
    
    hashFun->curr_hash_value = 0;
    for (int i = 0; i < len; i++) {
        hashFun->curr_hash_value = (a * hashFun->curr_hash_value + initialBytes[i]) % LARGE_PRIME;
    }
}

// Compute Hash
long hash_function(int a, const unsigned char* bytes, int len) {
    long hash_val = 0;
    for (int i = 0; i < len; i++) {
        hash_val = (a * hash_val + bytes[i]) % LARGE_PRIME;
    }
    return hash_val;
}

// Recompute hash when a sliding window moves
long recompute_hash(RollingHash *hashFun, unsigned char removed, unsigned char incoming) {
    hashFun->curr_hash_value = (hashFun->a * (hashFun->curr_hash_value - removed * hashFun->h) + incoming) % LARGE_PRIME;
    if (hashFun->curr_hash_value < 0) {
        hashFun->curr_hash_value += LARGE_PRIME;
    }
    return hashFun->curr_hash_value;
}

// Check for pattern in a file using rolling hash
int contains_bytes_file_rolling_hash(const unsigned char* pattern, int pattern_len, FILE* file) {
    unsigned char* buffer = (unsigned char*)malloc(pattern_len);
    if (!buffer) return 0;
    
    if (fread(buffer, 1, pattern_len, file) != pattern_len) {
        free(buffer);
        return 0;
    }
    
    RollingHash hashFun;
    init_rolling_hash(&hashFun, 31, buffer, pattern_len);
    long pattern_hash_val = hash_function(31, pattern, pattern_len);
    int count = 0;
    if (pattern_hash_val == hashFun.curr_hash_value) {
        printf("Pattern found at file position 0\n");
        count++;
    }
    
    int b, position = 1;
    while ((b = fgetc(file)) != EOF) {
        recompute_hash(&hashFun, buffer[0], (unsigned char)b);
        memmove(buffer, buffer + 1, pattern_len - 1);
        buffer[pattern_len - 1] = (unsigned char)b;
        
        if (pattern_hash_val == hashFun.curr_hash_value) {
            printf("Pattern found at file position %d\n", position);
            count++;
        }
        position++;
    }
    
    printf("Total occurrences in file: %d\n", count);
    free(buffer);
    return count > 0;
}

int main() {
    // Example usage
    unsigned char text[] = "This is a sample text for testing substring search. substring is here again.";
    unsigned char pattern[] = "substring";
    
    if (contains_bytes(pattern, strlen((char*)pattern), text, strlen((char*)text))) {
        printf("Pattern found using brute-force.\n");
    } else {
        printf("Pattern not found using brute-force.\n");
    }
    
    FILE* file = fopen("test.txt", "rb");
    if (file) {
        if (contains_bytes_file_rolling_hash(pattern, strlen((char*)pattern), file)) {
            printf("Pattern found in file using rolling hash.\n");
        } else {
            printf("Pattern not found in file using rolling hash.\n");
        }
        fclose(file);
    } else {
        printf("Error opening file.\n");
    }
    
    return 0;
}

