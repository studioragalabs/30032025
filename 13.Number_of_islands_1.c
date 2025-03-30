#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ISLAND '1'
#define WATER '0'

/* This C implementation provides two efficient approaches:

   DFS Approach (numIslands):
   - Recursively marks connected land ('1') as visited ('0').
   - Counts separate islands by iterating through the grid.
   - Suitable for small to medium input sizes.

   Union-Find Approach (numIslands2):
   - Dynamically processes "addLand" operations.
   - Uses Union-Find with path compression for efficiency.
   - Handles large grids incrementally.

   This implementation is optimized for both static and dynamic island counting.

   The segmentation fault is likely caused by directly modifying string literals in the DFS function, 
   as C treats string literals as read-only memory. 
   
   To fix this, we need to dynamically allocate the grid so it can be modified safely.
   
   Fixes:
   Dynamically allocate the grid:
   - Convert string literals into a mutable 2D array.
   - Use malloc() to allocate memory for each row.

   Properly pass a mutable grid to DFS:
   - Ensure the function modifies an allocated grid instead of a read-only string.

   Correct memory deallocation:
   - Free dynamically allocated memory at the end.

   
   Fixes & Improvements:
   Dynamically Allocated Grid (grid):
   - Now, grid is allocated using malloc(), making it modifiable.
   - Copies values from a static array (gridData) to ensure mutability.

   Memory Management:
   - Frees allocated memory after counting islands.
   - Prevents segmentation faults caused by modifying string literals.

   Corrected main() for Safe Execution:
   - Dynamically allocates memory for grid.
   - Copies grid contents correctly before passing to numIslands().

   Number of Islands (DFS): 1
   
   Number of Islands (Union-Find):
   1
   1
   2
   3

   the implementation runs without segmentation faults and correctly modifies the grid.

*/   


// Function to mark connected land as visited using DFS
void markIsland(char** grid, int rows, int cols, int i, int j) {
    if (i < 0 || i >= rows || j < 0 || j >= cols || grid[i][j] == WATER) return;
    
    grid[i][j] = WATER; // Mark as visited
    markIsland(grid, rows, cols, i - 1, j); // Up
    markIsland(grid, rows, cols, i + 1, j); // Down
    markIsland(grid, rows, cols, i, j - 1); // Left
    markIsland(grid, rows, cols, i, j + 1); // Right
}

// Function to count the number of islands
int numIslands(char** grid, int rows, int cols) {
    if (grid == NULL || rows == 0 || cols == 0) return 0;
    
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == ISLAND) {
                count++;
                markIsland(grid, rows, cols, i, j);
            }
        }
    }
    return count;
}

// Union-Find functions for handling large grids efficiently
int find(int* root, int id) {
    if (root[id] != id) root[id] = find(root, root[id]);
    return root[id];
}

int unionIslands(int* root, int* rank, int x, int y, int numIslands) {
    int rootX = find(root, x);
    int rootY = find(root, y);
    if (rootX != rootY) {
        numIslands--;
        if (rank[rootX] > rank[rootY]) {
            root[rootY] = rootX;
        } else if (rank[rootX] < rank[rootY]) {
            root[rootX] = rootY;
        } else {
            root[rootY] = rootX;
            rank[rootX]++;
        }
    }
    return numIslands;
}

// Function to count islands dynamically with addLand operations
void numIslands2(int rows, int cols, int positions[][2], int positionsSize) {
    int* root = (int*)malloc(rows * cols * sizeof(int));
    int* rank = (int*)malloc(rows * cols * sizeof(int));
    memset(root, -1, rows * cols * sizeof(int));
    memset(rank, 0, rows * cols * sizeof(int));
    
    int numIslands = 0;
    int directions[4][2] = {{0,1}, {1,0}, {-1,0}, {0,-1}};
    
    for (int p = 0; p < positionsSize; p++) {
        int i = positions[p][0], j = positions[p][1];
        int id = i * cols + j;
        if (root[id] != -1) {
            printf("%d\n", numIslands);
            continue; // Skip if already land
        }
        
        root[id] = id;
        rank[id] = 1;
        numIslands++;
        
        for (int d = 0; d < 4; d++) {
            int ni = i + directions[d][0];
            int nj = j + directions[d][1];
            int neighborId = ni * cols + nj;
            
            if (ni >= 0 && ni < rows && nj >= 0 && nj < cols && root[neighborId] != -1) {
                numIslands = unionIslands(root, rank, id, neighborId, numIslands);
            }
        }
        
        printf("%d\n", numIslands);
    }
    free(root);
    free(rank);
}

// Main function to test the implementation
int main() {
    int rows = 4, cols = 5;
    char gridData[4][5] = {
        {'1','1','1','1','0'},
        {'1','1','0','1','0'},
        {'1','1','0','0','0'},
        {'0','0','0','0','0'}
    };
    
    // Dynamically allocate the grid for modification
    char** grid = (char**)malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++) {
        grid[i] = (char*)malloc(cols * sizeof(char));
        memcpy(grid[i], gridData[i], cols * sizeof(char));
    }
    
    printf("Number of Islands (DFS): %d\n", numIslands(grid, rows, cols));
    
    // Free allocated grid memory
    for (int i = 0; i < rows; i++) {
        free(grid[i]);
    }
    free(grid);
    
    // Testing dynamic island counting
    int positions[][2] = {{0,0}, {0,1}, {1,2}, {2,1}};
    printf("\nNumber of Islands (Union-Find):\n");
    numIslands2(3, 3, positions, 4);
    
    return 0;
}
