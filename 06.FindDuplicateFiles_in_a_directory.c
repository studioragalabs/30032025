#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/sha.h>

#define BUFFER_SIZE 1024

// Function to compute SHA-256 hash of a file
char* hash_file(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    
    unsigned char buffer[BUFFER_SIZE];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    
    fclose(file);
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);
    
    char* hashString = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hashString + (i * 2), "%02x", hash[i]);
    }
    hashString[SHA256_DIGEST_LENGTH * 2] = '\0';
    
    return hashString;
}

// Function to get all files recursively
void get_all_files(const char* path, FILE* outputFile) {
    struct dirent *entry;
    struct stat fileStat;
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Error opening directory");
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
        
        if (lstat(fullPath, &fileStat) == -1) {
            perror("Error getting file status");
            continue;
        }
        
        if (S_ISREG(fileStat.st_mode)) { // If it's a regular file
            char* hash = hash_file(fullPath);
            if (hash) {
                fprintf(outputFile, "%s %s\n", hash, fullPath);
                free(hash);
            }
        } else if (S_ISDIR(fileStat.st_mode)) { // If it's a directory
            get_all_files(fullPath, outputFile);
        }
    }
    
    closedir(dir);
}

// Function to find duplicates from a stored hash map
void find_duplicates(const char* directory) {
    FILE* tempFile = fopen("file_hashes.txt", "w");
    if (!tempFile) {
        perror("Error creating temporary file");
        return;
    }
    
    get_all_files(directory, tempFile);
    fclose(tempFile);
    
    // Read stored hashes and detect duplicates
    FILE* inputFile = fopen("file_hashes.txt", "r");
    if (!inputFile) {
        perror("Error opening hash file");
        return;
    }
    
    char hash[SHA256_DIGEST_LENGTH * 2 + 1];
    char filePath[1024];
    
    struct HashNode {
        char* hash;
        char* filePath;
        struct HashNode* next;
    };
    
    struct HashNode* hashTable[10000] = {NULL};
    
    while (fscanf(inputFile, "%s %s", hash, filePath) != EOF) {
        int index = (hash[0] + hash[1]) % 10000; // Simple hash function
        struct HashNode* newNode = malloc(sizeof(struct HashNode));
        newNode->hash = strdup(hash);
        newNode->filePath = strdup(filePath);
        newNode->next = hashTable[index];
        hashTable[index] = newNode;
    }
    fclose(inputFile);
    
    // Print duplicates
    printf("Duplicate files found:\n");
    for (int i = 0; i < 10000; i++) {
        struct HashNode* current = hashTable[i];
        while (current) {
            struct HashNode* runner = current->next;
            while (runner) {
                if (strcmp(current->hash, runner->hash) == 0) {
                    printf("%s and %s are duplicates\n", current->filePath, runner->filePath);
                }
                runner = runner->next;
            }
            current = current->next;
        }
    }
    
    // Cleanup memory
    for (int i = 0; i < 10000; i++) {
        struct HashNode* current = hashTable[i];
        while (current) {
            struct HashNode* temp = current;
            current = current->next;
            free(temp->hash);
            free(temp->filePath);
            free(temp);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <directory path>\n", argv[0]);
        return 1;
    }
    
    find_duplicates(argv[1]);
    return 0;
}
