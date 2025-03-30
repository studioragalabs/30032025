#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Trie Structure (TrieNode): Efficient prefix-based word search.
   Word Insertion (insertWord): Adds words to the Trie.
   Word Search (searchWord): Checks if a word exists in the Trie.
   Prefix Search (startsWith): Helps optimize DFS traversal.
   Phone Keypad Mapping (KEYS): Maps digits to letter groups.
   Recursive DFS (findCombinations): Generates valid words from digits.
   Wrapper Function (letterCombinations): Retrieves all valid word combinations.
   Main Function (main): Demonstrates dictionary word matching with the number 3767269.
   
   This approach efficiently filters invalid paths in O(1) prefix lookup time, 
   reducing unnecessary DFS calls
*/

#define MAX_CHILDREN 26  // Alphabet size
#define MAX_DIGITS 10    // Digits 0-9

// Trie Node structure
typedef struct TrieNode {
    struct TrieNode* children[MAX_CHILDREN];
    bool isWord;
} TrieNode;

// Initialize a new Trie node
TrieNode* createTrieNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (node) {
        node->isWord = false;
        for (int i = 0; i < MAX_CHILDREN; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

// Insert a word into the Trie
void insertWord(TrieNode* root, const char* word) {
    TrieNode* curr = root;
    while (*word) {
        int index = *word - 'a';
        if (curr->children[index] == NULL) {
            curr->children[index] = createTrieNode();
        }
        curr = curr->children[index];
        word++;
    }
    curr->isWord = true;
}

// Search for a complete word in the Trie
bool searchWord(TrieNode* root, const char* word) {
    TrieNode* curr = root;
    while (*word) {
        int index = *word - 'a';
        if (curr->children[index] == NULL) return false;
        curr = curr->children[index];
        word++;
    }
    return curr->isWord;
}

// Check if there is any word that starts with the given prefix
bool startsWith(TrieNode* root, const char* prefix) {
    TrieNode* curr = root;
    while (*prefix) {
        int index = *prefix - 'a';
        if (curr->children[index] == NULL) return false;
        curr = curr->children[index];
        prefix++;
    }
    return true;
}

// Phone keypad mapping
const char* KEYS[MAX_DIGITS] = {"", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};

// Recursive function to find valid word combinations
void findCombinations(TrieNode* root, TrieNode* node, char* digits, char* buffer, int index, int depth, char** results, int* resultCount) {
    if (digits[index] == '\0') {
        if (node->isWord) {
            buffer[depth] = '\0';
            results[*resultCount] = strdup(buffer);
            (*resultCount)++;
        }
        return;
    }
    
    const char* keyLetters = KEYS[digits[index] - '0'];
    for (int i = 0; keyLetters[i] != '\0'; i++) {
        int charIndex = keyLetters[i] - 'a';
        if (node->children[charIndex]) {
            buffer[depth] = keyLetters[i];
            findCombinations(root, node->children[charIndex], digits, buffer, index + 1, depth + 1, results, resultCount);
        }
    }
}

// Wrapper function to get letter combinations
char** letterCombinations(TrieNode* root, const char* digits, int* returnSize) {
    char** results = (char**)malloc(100 * sizeof(char*));  // Allocating space for results
    *returnSize = 0;
    if (!digits || digits[0] == '\0') return results;
    
    char buffer[100];  // Temporary buffer for word formation
    findCombinations(root, root, (char*)digits, buffer, 0, 0, results, returnSize);
    return results;
}

// Main function for testing
int main() {
    TrieNode* root = createTrieNode();
    
    // Insert dictionary words
    insertWord(root, "drop");
    insertWord(root, "box");
    insertWord(root, "dropbox");
    
    int resultSize;
    char** combinations = letterCombinations(root, "3767269", &resultSize);
    
    printf("Valid words from phone number:\n");
    for (int i = 0; i < resultSize; i++) {
        printf("%s\n", combinations[i]);
        free(combinations[i]);
    }
    free(combinations);
    
    return 0;
}

