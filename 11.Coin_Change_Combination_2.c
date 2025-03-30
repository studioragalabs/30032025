#include <stdio.h>
#include <stdlib.h>

/* This C implementation follows the original Java logic:

   Recursive Backtracking Approach (findCombinations):
   - Uses DFS to explore all possible combinations.
   - Avoids duplicate combinations by ensuring elements are always picked in non-decreasing order.

   Dynamic Memory Allocation:
   - Uses structures (Combination, ResultSet) to handle dynamic lists of numbers.
   - Allocates memory for each combination dynamically to store unique results.

   Efficient Result Storage (addCombination):
   - Expands the result set dynamically.
   - Copies valid combinations into a structured format.

   Utility Functions:
   - printResultSet(): Prints the list of valid soda purchase combinations.
   - freeResultSet(): Frees allocated memory to prevent leaks.

   Example Output for {2, 3, 6, 7}, Target = 7:

   [
     [7]
     [2, 2, 3]
   ]
   
   This implementation ensures efficient memory usage while maintaining correct combinatorial logic
*/   
/* Improvements in the Updated Implementation:

   Sorting the Final Results:
   - The compareCombinations() function ensures results are sorted:
     - Descending by length (longer combinations come first).
     - Lexicographically for equal-length combinations.

   Sorting Candidates Before Processing:
   - The qsort() function sorts candidates[] before recursion, ensuring:
     - The output remains in a consistent order.
     - The recursive calls always generate valid, sorted sequences.

   Ensuring [7] Comes Before [2,2,3]:
   - The sorting ensures the result follows the expected order.

*/   
/* Your output is correct, but the order is still incorrect ([3, 2, 2] should be [2, 2, 3]). 
 * To ensure that the output is properly sorted in lexicographical order, I'll make the following fixes:

 Improvements:
 - Sort Candidates in Ascending Order Before Processing:
   - This ensures that 2 appears before 3 in the recursive function.
   - The qsort() function should sort candidates[] in ascending order.

 - Modify Sorting Function for Result Combinations:
   - The compareCombinations() function needs to sort numbers within each combination in ascending order.

 Improvements in the Updated Code
 
 Sorting Candidates in Ascending Order Before Processing:
 - Ensures that smaller numbers (like 2) are always considered before larger numbers (3, 6, 7).
 - Uses qsort() with compareInts() to achieve ascending sorting.

 Sorting Each Combination Before Adding to Results:
 - Inside addCombination(), the combination is sorted before storing to maintain lexicographical order.
 - Uses qsort() to sort elements in each combination.

 Sorting the Final Result Set Properly:
 - Uses qsort() to sort results based on:
   - Combination length (shorter combinations first).
   - Lexicographical order (numbers are compared sequentially).  

 Expected Output for {2,3,6,7} with target = 7:
  
 [
   [7]
   [2, 2, 3]
 ]
*/ 


// Structure to store a list of numbers
typedef struct {
    int* array;
    int size;
} Combination;

// Structure to store a list of combinations
typedef struct {
    Combination* combinations;
    int size;
    int capacity;
} ResultSet;

// Function to compare two integers (for qsort)
int compareInts(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// Function to compare two combinations for sorting results
int compareCombinations(const void* a, const void* b) {
    Combination* combA = (Combination*)a;
    Combination* combB = (Combination*)b;
    if (combA->size != combB->size) {
        return combA->size - combB->size; // Sort by length in ascending order
    }
    for (int i = 0; i < combA->size; i++) {
        if (combA->array[i] != combB->array[i]) {
            return combA->array[i] - combB->array[i]; // Sort lexicographically
        }
    }
    return 0;
}

// Function to initialize the result set
void initResultSet(ResultSet* resultSet) {
    resultSet->size = 0;
    resultSet->capacity = 10;
    resultSet->combinations = (Combination*)malloc(resultSet->capacity * sizeof(Combination));
}

// Function to add a new combination to the result set
void addCombination(ResultSet* resultSet, int* combination, int size) {
    if (resultSet->size == resultSet->capacity) {
        resultSet->capacity *= 2;
        resultSet->combinations = (Combination*)realloc(resultSet->combinations, resultSet->capacity * sizeof(Combination));
    }
    
    resultSet->combinations[resultSet->size].array = (int*)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        resultSet->combinations[resultSet->size].array[i] = combination[i];
    }
    resultSet->combinations[resultSet->size].size = size;
    
    // Sort the combination before adding it to maintain lexicographical order
    qsort(resultSet->combinations[resultSet->size].array, size, sizeof(int), compareInts);
    
    resultSet->size++;
}

// Recursive function to find all combinations
void findCombinations(ResultSet* resultSet, int* candidates, int candidatesSize, int target, int* currentCombination, int size, int index) {
    if (target < 0) return;
    if (target == 0) {
        addCombination(resultSet, currentCombination, size);
        return;
    }
    
    for (int i = index; i < candidatesSize; i++) {
        currentCombination[size] = candidates[i];
        findCombinations(resultSet, candidates, candidatesSize, target - candidates[i], currentCombination, size + 1, i);
    }
}

// Wrapper function to find all combinations
ResultSet combinationSum(int* candidates, int candidatesSize, int target) {
    ResultSet resultSet;
    initResultSet(&resultSet);
    
    // Sort the candidates array in ascending order before processing
    qsort(candidates, candidatesSize, sizeof(int), compareInts);
    
    int* currentCombination = (int*)malloc(target * sizeof(int)); // Max possible size
    findCombinations(&resultSet, candidates, candidatesSize, target, currentCombination, 0, 0);
    free(currentCombination);
    
    // Sort the final result set before returning
    qsort(resultSet.combinations, resultSet.size, sizeof(Combination), compareCombinations);
    
    return resultSet;
}

// Function to print results
void printResultSet(ResultSet resultSet) {
    printf("[\n");
    for (int i = 0; i < resultSet.size; i++) {
        printf("  [");
        for (int j = 0; j < resultSet.combinations[i].size; j++) {
            printf("%d", resultSet.combinations[i].array[j]);
            if (j < resultSet.combinations[i].size - 1) printf(", ");
        }
        printf("]\n");
    }
    printf("]\n");
}

// Free memory allocated for the result set
void freeResultSet(ResultSet resultSet) {
    for (int i = 0; i < resultSet.size; i++) {
        free(resultSet.combinations[i].array);
    }
    free(resultSet.combinations);
}

int main() {
    int candidates[] = {2, 3, 6, 7};
    int target = 7;
    int candidatesSize = sizeof(candidates) / sizeof(candidates[0]);
    
    ResultSet resultSet = combinationSum(candidates, candidatesSize, target);
    printResultSet(resultSet);
    freeResultSet(resultSet);
    
    return 0;
}
