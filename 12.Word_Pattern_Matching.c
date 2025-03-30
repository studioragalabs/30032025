#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* This C implementation covers three key functionalities:

   Word Pattern Matching (wordPattern):
   - Maps characters to words in a string.
   - Ensures a bijection mapping (each pattern character uniquely maps to one word).
   - Uses a lookup table to verify mappings.

   Word Pattern Match with Recursion (wordPatternMatch):
   - Uses backtracking to check if a pattern can match a string dynamically.
   - Supports arbitrary-length word mapping.

   Word Break (wordBreak):
   - Implements Dynamic Programming (DP) to check if a string can be broken into dictionary words.
   - Uses a boolean DP table to determine valid segmentations.

   Word Pattern Matching:
   1
   0

   Word Pattern Match (Recursive):
   1

   Word Break:
   1
   
   This ensures a correct and efficient mapping implementation.
*/   


#define MAX_CHAR 256

// Function to check if a pattern follows a word mapping
bool wordPattern(const char* pattern, const char* str) {
    char* patternStrMap[MAX_CHAR] = {NULL};
    char words[100][50];  // Assuming max 100 words, each up to 50 chars
    int wordCount = 0;
    
    // Tokenizing the input string
    char* token = strtok(strdup(str), " ");
    while (token) {
        strcpy(words[wordCount++], token);
        token = strtok(NULL, " ");
    }
    
    if (strlen(pattern) != wordCount) return false;
    
    bool usedWords[100] = {false};  // To check if a word is already mapped
    
    for (int i = 0; i < strlen(pattern); i++) {
        char c = pattern[i];
        if (patternStrMap[c] == NULL) {
            // Check if the word is already mapped
            for (int j = 0; j < i; j++) {
                if (strcmp(words[j], words[i]) == 0) return false;
            }
            patternStrMap[c] = strdup(words[i]);
        } else {
            if (strcmp(patternStrMap[c], words[i]) != 0) return false;
        }
    }
    return true;
}

// Recursive function to check if word pattern matches
bool isMatch(char pattern[], int pi, char* str, int si, char mapping[MAX_CHAR][50], bool used[100]) {
    if (pattern[pi] == '\0' && str[si] == '\0') return true;
    if (pattern[pi] == '\0' || str[si] == '\0') return false;
    
    char c = pattern[pi];
    if (strlen(mapping[c]) > 0) {
        int len = strlen(mapping[c]);
        if (strncmp(str + si, mapping[c], len) == 0)
            return isMatch(pattern, pi + 1, str, si + len, mapping, used);
        return false;
    }
    
    for (int k = si; k < strlen(str); k++) {
        char word[50];
        strncpy(word, str + si, k - si + 1);
        word[k - si + 1] = '\0';
        
        if (used[k]) continue;
        
        strcpy(mapping[c], word);
        used[k] = true;
        if (isMatch(pattern, pi + 1, str, k + 1, mapping, used)) return true;
        mapping[c][0] = '\0';
        used[k] = false;
    }
    return false;
}

bool wordPatternMatch(char pattern[], char str[]) {
    char mapping[MAX_CHAR][50] = {0};
    bool used[100] = {false};
    return isMatch(pattern, 0, str, 0, mapping, used);
}

// Word Break Problem
bool wordBreak(char* s, char* wordDict[], int wordCount) {
    int len = strlen(s);
    bool dp[len + 1];
    memset(dp, false, sizeof(dp));
    dp[0] = true;
    
    for (int i = 1; i <= len; i++) {
        for (int j = 0; j < i; j++) {
            for (int k = 0; k < wordCount; k++) {
                if (dp[j] && strncmp(s + j, wordDict[k], i - j) == 0) {
                    dp[i] = true;
                    break;
                }
            }
        }
    }
    return dp[len];
}

int main() {
    printf("Word Pattern Matching:\n");
    printf("%d\n", wordPattern("abba", "dog cat cat dog")); // Expected output: 1 (true)
    printf("%d\n", wordPattern("abba", "dog cat cat fish")); // Expected output: 0 (false)
    
    printf("\nWord Pattern Match (Recursive):\n");
    printf("%d\n", wordPatternMatch("abba", "dogcatcatdog")); // Expected output: 1 (true)
    
    printf("\nWord Break:\n");
    char* wordDict[] = {"leet", "code"};
    printf("%d\n", wordBreak("leetcode", wordDict, 2)); // Expected output: 1 (true)
    
    return 0;
}
