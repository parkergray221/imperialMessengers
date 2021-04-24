#include <stdio.h>
#include <stdlib.h>

int dijkstra(int n, int adjMat[][n]);
void adjMatBuilder(int n, int adjMat[][n], FILE* input);

int main(int argc, char* argv []){
    // Can pass filename as argument or use stdin to input data manually

    int size; // defines row and column bounds for input adjacency matrix
    int fileRead = 0; // denotes if we're using file from argument

    FILE* input = fopen(argv[1], "r"); // open file for reading

    if(input != NULL){
        printf("Successfully opened input file.\n");
        fileRead = 1;
        fscanf(input, "%d", &size); // grab first line of file to get our matrix row/col size 
    } else {
        printf("No file was provided as argument. Please enter data manually.\n");
        printf("First, enter the row/col count for our nxn matrix: ");
        scanf("%i", &size);
    }
    
    if(size == 1){
        printf("Lowest amount of time to travel to 1 city is %d\n", 0);
        return 0; // if there's only one city in the empire (and therefore only one city to start from), the min distance is 0. 
    }

    int empireAdjMatrix[size][size]; // declare 2d matrix with size based on first line
    
    for(int i = 0; i < size; i++){ // initialize values of empireAdjMatrix to 0
        for(int j = 0; j < size; j++){
            empireAdjMatrix[i][j] = 0;
        }
    }

    adjMatBuilder(size, empireAdjMatrix, input); // fill matrix with data from file or from stdin

    printf("Input adjacency matrix has row/col dimensions of %dx%d\n", size, size);
    printf("Adjacency matrix has the following values:\n\n");
    // verifying that I copied the input data over correctly 
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if(empireAdjMatrix[i][j] == INT_MAX){
                printf("x\t");
            } else {
                printf("%d\t", empireAdjMatrix[i][j]);
            }    
        }
        printf("\n");
    }
    printf("\n");

    int ans = dijkstra(size, empireAdjMatrix);
    printf("Lowest amount of time to travel to %d cities is %d\n", size, ans);
    printf("Finished");
}

void adjMatBuilder(int n, int adjMat[][n], FILE* input){
    if(input != NULL){ // read from file case
        printf("Reading input file...\n");
        char indexBuffer[128];
        int row = 1; // input file starts on row 1 rather than 0 of the matrix
        int i = 1, j = 0; // our row/col indexers for adj matrix

        while(fscanf(input, "%256s", indexBuffer) == 1){
            if(j == row){ // advance to the next row if we reach the diagonal bound of a row
                row++; 
                i++;
                j = 0;
            }

            if(indexBuffer[0] == 'x' || indexBuffer[0] == 'X'){ // handle char x case
                adjMat[i][j] = INT_MAX; // make the index with x unpickable as far as the search alg is concerned.
                adjMat[j][i] = INT_MAX; // this line wasn't present in first submission to luke/leftos

            } else { // int case
                int distance = (int) atoi(indexBuffer);
                adjMat[i][j] = distance; // A[i][j] == A[j][i] according to challenge prompt
                adjMat[j][i] = distance;
                
            }
            j++;
        }

    } else { // read from stdin case
        printf("Next, enter the lower triangular portion of the adjacency matrix as prompted.\n");
        char indexBuffer[128];

        for(int row = 1; row < n; row++){
            for(int col = 0; col < row; col++){
                printf("Enter adjMat[%d][%d]: ", row, col);
                scanf("%256s", indexBuffer);
                if(indexBuffer[0] == 'x' || indexBuffer[0] == 'X'){
                    adjMat[row][col] = INT_MAX;
                    adjMat[col][row] = INT_MAX;
                } else {
                    int distance = (int) atoi(indexBuffer);
                    adjMat[row][col] = distance; // A[i][j] == A[j][i] according to challenge prompt
                    adjMat[col][row] = distance;
                };
            }
        }
    }
}

/*
    Dijkstra's algorithm implementation:

    1. Mark all nodes as unvisited. Put min distance to each node as INT_MAX. 
    
    2. Pick a node to start from. Set that node's min distance to 0, as it's our starting node.

    3. For the current node, check the node's adjacency list data vs the existing min dist to each node. If the current node's adjacency data for some city x is lower
    than the existing data for city x, update that city's data with the new lowest min distance.

    4. Once all of a node's edges have been researched, mark it as no longer relevant. Repeat step 3 with the next node.

    5. When all nodes have been fully visited, halt the algorithm.

    For the given sample_input below,
    0	50	30	100	10		row 0. city 1

    50	0	5	20	x		row 1. city 2

    30	5	0	50	x		row 2. city 3

    100	20	50	0	10		row 3. city 4

    10	x	x	10	0		row 4. city 5
    
    the algorithm should operate like so:

    Pick city 1 -> city 5 path for 10.
    Pick city 5 -> city 4 path for 10. (total of 20 thus far)
    Examine city 4's choices- find that there doesn't exist a path from city 4 to an unvisited city that is less than city 1 -> city 3 for 30. 
    We do not increment the 20 travel time used for the 1 -> 5 -> 4 traversal because this total of 20 could occur 'during' the traversal from 1 -> 3 for 30.
    Finally, note that 3 -> 2 (an addition of 5) is the next cheapest path to an unvisited city.
    The duration used is a total of 35. 

    c1                                              
                                                            c2                                            
                                                    c3                                
                                    c4
                    c5
    0       5       10      15      20      25      30      35

*/

int dijkstra(int n, int adjMat[][n]){    

    int minTravel[n]; // denotes shortest travel time to each city
    int visited[n]; // 0 for unvisited, 1 for visited. a city is visited if all of its adjacency data has been evaluated against the existing data.
    for(int i = 0; i < n; i++){
        minTravel[i] = INT_MAX;
        visited[i] = 0;
    }    
    
    int currentCity = 0; // start at city 1, our capital 
    minTravel[0] = 0; 
    int visitCount = 0;
    int min;

    while(visitCount < n){ 
        min = INT_MAX; 

        for(int i = 0; i < n; i++){ // iterate through current city's adjacency list
            if(i != currentCity && visited[i] == 0 && adjMat[currentCity][i] != INT_MAX){ 
                // for any given city, we don't care about its distance from itself, cities that are inaccessible from our given city or the given city's distance from visited cities that we've already evaluated.

                if(adjMat[currentCity][i] + minTravel[currentCity] < minTravel[i]){ // adjacency matrix evaluation. 
                // if the time it takes to get to the current city + travel from the current city to some city i is less than the current on record fastest time to get to that city, update minTravel[i]
                    minTravel[i] = adjMat[currentCity][i] + minTravel[currentCity];  

                }                             
            }
        }

        visited[currentCity] = 1;
        visitCount++;

        for(int j = 0; j < n; j++){ // determining the next city to evaluate
            if(minTravel[j] < min && visited[j] == 0){
                currentCity = j; // should travel to the city with the lowest minTravel value
            }
        } 
    }

    printf("Minimum travel distance to each city starting from capital:\n");
    for(int i = 0; i < n; i++){
        printf("%d\t", minTravel[i]);
    }
    printf("\n");

    // one last loop....
    int travelTotal = 0;
    for(int i = 0; i < n; i++){
        if(minTravel[i] > travelTotal){
            travelTotal = minTravel[i];
        }
    }

    return travelTotal;
}
