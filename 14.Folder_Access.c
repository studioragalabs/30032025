#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* This C implementation follows the Java logic and includes:

   Folder Hierarchy Management (addFolder):
   - Stores child-parent relationships in a structured format.
   
   Access Control (addAccess, hasAccess):
   - Checks if a folder is accessible by traversing up the hierarchy.

   Optimized Access List (simplifyAccess):
   - Removes redundant access entries by checking inherited permissions.

   Efficient Lookups (getParent, isAccessible):
   - Uses linear search (can be optimized with hash maps for large data).

   Memory Management:
   - Uses fixed-size arrays for simplicity.
   - Could be improved with dynamic memory allocation for scalability.

   Expected Output:

   Access to B: false
   Access to C: true
   Access to F: true
   Access to G: false

   Simplified Access List:
   C
   E

   This ensures correct and efficient folder access checking.

*/   

#define MAX_FOLDERS 100

// Structure to represent the folder hierarchy
typedef struct {
    char folderName[50];
    char parentName[50];
} Folder;

// Structure to manage folder access
typedef struct {
    Folder folders[MAX_FOLDERS];
    int folderCount;
    char access[MAX_FOLDERS][50];
    int accessCount;
} FolderAccess;

// Function to initialize FolderAccess
void initFolderAccess(FolderAccess* fa) {
    fa->folderCount = 0;
    fa->accessCount = 0;
}

// Function to add a folder relationship
void addFolder(FolderAccess* fa, const char* folder, const char* parent) {
    strcpy(fa->folders[fa->folderCount].folderName, folder);
    if (parent != NULL) {
        strcpy(fa->folders[fa->folderCount].parentName, parent);
    } else {
        strcpy(fa->folders[fa->folderCount].parentName, "");
    }
    fa->folderCount++;
}

// Function to add an accessible folder
void addAccess(FolderAccess* fa, const char* folder) {
    strcpy(fa->access[fa->accessCount], folder);
    fa->accessCount++;
}

// Function to find the parent of a folder
const char* getParent(FolderAccess* fa, const char* folder) {
    for (int i = 0; i < fa->folderCount; i++) {
        if (strcmp(fa->folders[i].folderName, folder) == 0) {
            return fa->folders[i].parentName;
        }
    }
    return NULL; // Folder not found
}

// Function to check if a folder is in the access list
bool isAccessible(FolderAccess* fa, const char* folder) {
    for (int i = 0; i < fa->accessCount; i++) {
        if (strcmp(fa->access[i], folder) == 0) {
            return true;
        }
    }
    return false;
}

// Function to check if a user has access to a folder
bool hasAccess(FolderAccess* fa, const char* folder) {
    const char* currFolder = folder;
    while (currFolder != NULL && strcmp(currFolder, "") != 0) {
        if (isAccessible(fa, currFolder)) {
            return true;
        }
        currFolder = getParent(fa, currFolder);
    }
    return false;
}

// Function to simplify access list
void simplifyAccess(FolderAccess* fa) {
    bool keep[MAX_FOLDERS] = {false};
    for (int i = 0; i < fa->accessCount; i++) {
        const char* currFolder = getParent(fa, fa->access[i]);
        bool shouldDelete = false;
        while (currFolder != NULL && strcmp(currFolder, "") != 0) {
            if (isAccessible(fa, currFolder)) {
                shouldDelete = true;
                break;
            }
            currFolder = getParent(fa, currFolder);
        }
        if (!shouldDelete) {
            keep[i] = true;
        }
    }
    // Create a new access list with only non-redundant entries
    char newAccess[MAX_FOLDERS][50];
    int newCount = 0;
    for (int i = 0; i < fa->accessCount; i++) {
        if (keep[i]) {
            strcpy(newAccess[newCount++], fa->access[i]);
        }
    }
    // Copy the new access list back
    fa->accessCount = newCount;
    for (int i = 0; i < newCount; i++) {
        strcpy(fa->access[i], newAccess[i]);
    }
}

// Main function for testing
int main() {
    FolderAccess fa;
    initFolderAccess(&fa);
    
    // Define folder hierarchy
    addFolder(&fa, "A", NULL);
    addFolder(&fa, "B", "A");
    addFolder(&fa, "C", "B");
    addFolder(&fa, "D", "B");
    addFolder(&fa, "E", "A");
    addFolder(&fa, "F", "E");
    
    // Define access list
    addAccess(&fa, "C");
    addAccess(&fa, "E");
    
    // Test hasAccess function
    printf("Access to B: %s\n", hasAccess(&fa, "B") ? "true" : "false");
    printf("Access to C: %s\n", hasAccess(&fa, "C") ? "true" : "false");
    printf("Access to F: %s\n", hasAccess(&fa, "F") ? "true" : "false");
    printf("Access to G: %s\n", hasAccess(&fa, "G") ? "true" : "false");
    
    // Simplify access list
    simplifyAccess(&fa);
    printf("\nSimplified Access List:\n");
    for (int i = 0; i < fa.accessCount; i++) {
        printf("%s\n", fa.access[i]);
    }
    
    return 0;
}

