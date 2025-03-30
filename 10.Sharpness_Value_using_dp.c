#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Standard DP Approach (findSharpnessValue):
   - Uses a 2D table to store sharpness values.
   - Computes min(max(...)) from left, upper-left, and lower-left paths.
   - Finds the maximum sharpness value in the last column.

   Optimized DP Approach (findSharpnessValueOptimized):
   - Reduces space complexity from O(m × n) to O(m).
   - Uses a single array instead of a full DP table.
   - Keeps track of previous column values to update the next column.

   Memory Management:
   - Dynamically allocates memory for the DP tables.
   - Frees allocated memory after computation.

   Test Cases in main():
   - Initializes a sample 3×3 matrix.
   - Computes sharpness values using both methods.
   - Ensures proper memory deallocation.

   This is optimized for large matrices while keeping accurate DP logic.
*/   

// Function to compute the maximum sharpness value using DP
int findSharpnessValue(int** matrix, int m, int n) {
    if (matrix == NULL || m < 1 || n < 1) return -1;
    
    // Allocate memory for sharpness values
    int** sharpness = (int**)malloc(m * sizeof(int*));
    for (int i = 0; i < m; i++) {
        sharpness[i] = (int*)malloc(n * sizeof(int));
    }
    
    // Initialize the first column with matrix values
    for (int i = 0; i < m; i++) {
        sharpness[i][0] = matrix[i][0];
    }
    
    // Compute DP values for sharpness
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < m; i++) {
            int pathPrev = sharpness[i][j - 1];
            if (i > 0) {
                pathPrev = (pathPrev > sharpness[i - 1][j - 1]) ? pathPrev : sharpness[i - 1][j - 1];
            }
            if (i < m - 1) {
                pathPrev = (pathPrev > sharpness[i + 1][j - 1]) ? pathPrev : sharpness[i + 1][j - 1];
            }
            sharpness[i][j] = (pathPrev < matrix[i][j]) ? pathPrev : matrix[i][j];
        }
    }
    
    // Find the max sharpness value in the last column
    int maxSharpness = INT_MIN;
    for (int i = 0; i < m; i++) {
        if (sharpness[i][n - 1] > maxSharpness) {
            maxSharpness = sharpness[i][n - 1];
        }
    }
    
    // Free allocated memory
    for (int i = 0; i < m; i++) {
        free(sharpness[i]);
    }
    free(sharpness);
    
    return maxSharpness;
}

// Space optimized DP function
int findSharpnessValueOptimized(int** matrix, int m, int n) {
    if (matrix == NULL || m < 1 || n < 1) return -1;
    
    int* sharpness = (int*)malloc(m * sizeof(int));
    for (int i = 0; i < m; i++) {
        sharpness[i] = matrix[i][0];
    }
    
    for (int j = 1; j < n; j++) {
        int prev = sharpness[0];
        for (int i = 0; i < m; i++) {
            int maxPath = sharpness[i];
            if (i > 0) {
                maxPath = (maxPath > prev) ? maxPath : prev;
            }
            if (i < m - 1) {
                maxPath = (maxPath > sharpness[i + 1]) ? maxPath : sharpness[i + 1];
            }
            prev = sharpness[i];
            sharpness[i] = (maxPath < matrix[i][j]) ? maxPath : matrix[i][j];
        }
    }
    
    int maxSharpness = INT_MIN;
    for (int i = 0; i < m; i++) {
        if (sharpness[i] > maxSharpness) {
            maxSharpness = sharpness[i];
        }
    }
    
    free(sharpness);
    return maxSharpness;
}

// Test the function
int main() {
    int m = 3, n = 3;
    int matrixData[3][3] = {{5, 7, 2}, {7, 5, 8}, {9, 1, 5}};
    
    // Allocate memory for matrix
    int** matrix = (int**)malloc(m * sizeof(int*));
    for (int i = 0; i < m; i++) {
        matrix[i] = (int*)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            matrix[i][j] = matrixData[i][j];
        }
    }
    
    printf("Max Sharpness Value (DP): %d\n", findSharpnessValue(matrix, m, n));
    printf("Max Sharpness Value (Optimized): %d\n", findSharpnessValueOptimized(matrix, m, n));
    
    // Free allocated memory
    for (int i = 0; i < m; i++) {
        free(matrix[i]);
    }
    free(matrix);
    
    return 0;
}

