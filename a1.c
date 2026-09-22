/*
Name: Zaine Ancheta
UCID: 30214484
Tutorial: T14
TA: Zahra Arabi

Assignment 1
*/

#include <stdlib.h> // Standard header file for C
#include <stdio.h> // Header file for printf function
#include <sys/types.h> // Header file for process IDs (PID)
#include <unistd.h> // Header file for fork(), getpid(), and getppid()
#include <sys/wait.h> // Header file for wait() and waitpid()

#define MAX_CHILDREN 8
pid_t childPIDs[MAX_CHILDREN];
int pipes[MAX_CHILDREN][2];
int childrenCreated = 0;

// Function that computes to the nth Fibonacci number
int fibonacci(int n)
{
    int previous;
    int current;
    int next;
    int i;

    // Base case for Fibonacci sequence
    if (n == 0)
    {
        return 0;
    }
    if (n == 1)
    {
        return 1;
    }
    // Iterative approach for the fibonacci sequence
    previous = 0;
    current = 1;
    next = 0;
    for (i = 2; i <= n; i++)
    {
        next = previous + current;
        previous = current;
        current = next;
    }
    return current;
}


// Function that creates a child process to compute a Fibonacci number
void createChildProcess(int n)
{
    pid_t childPID;
    int childIndex;
    int result;
    childIndex = childrenCreated;
    // Create pipe for this child process
    if (pipe(pipes[childIndex]) < 0) 
    {
        perror("pipe failed");
        exit(1);
    }
    // Reference, Zahra Arabi, "fork" slide 4. (Lines 72-96)
    // I used her code from slide 4 to develop structure for creating the rest of the code in this function. 
    // More specifically, the structure for a basic fork, and then incorporating the fibonacci function.

    // Create child process, assignment wants in parallel so we fork the child processes here
    childPID = fork();
    // Check if the fork fails
    if (childPID < 0)
    {
        perror("fork failed");
        exit(1);
    }
    // If not, proceed and call the fibonacci function and have the child output its result
    if (childPID == 0)
    {
        // Child does not need to read from the pipe
        close(pipes[childIndex][0]);
        result = fibonacci(n);
        // Send the result to the parent process through the pipe
        write(pipes[childIndex][1], &result, sizeof(result));
        // Close the writing end of the pipe
        close(pipes[childIndex][1]);
        // Exit to prevent the child from creating more processes
        exit(0);
    }
    // Save the child process ID
    childPIDs[childIndex] = childPID;
    // Parent does not need to write to the pipe
    close(pipes[childIndex][1]);
    childrenCreated++;
}

// Function that waits for all child processes to finish
void waitForChildren(int numberOfChildren)
{
    int i;
    // Run for all child processes that were made
    for (i = 0; i < numberOfChildren; i++)
    {
        // Once children are all done processing
        waitpid(childPIDs[i], NULL, 0);
    }
    // Then parent process can stop, using wait() or waitpid()
}


int main(int argc, char *argv[]) // Main function, argc is number of command-line arguments, argv stores them
{
    int numberOfChildren; // Stores how many child processes need to be created
    int fibonacciInputs[MAX_CHILDREN]; // Stores each Fibonacci input from the command line
    int result; // Stores a Fibonacci result read from a child process
    int i; // Loop counter
    // Take arguments
    if (argc < 2) // Check that at least one Fibonacci number was provided
    {
        printf("Usage: %s <n1> <n2> ... <n8>\n", argv[0]); // Show correct program usage
        return 1; // End program because no input was given
    }
    if (argc > 9) // Checks if more arguments were passed than required.
    {
        printf("Error: Maximum of 8 arguments allowed.\n"); // Tell user they entered too many values
        return 1; // End program because input is invalid
    }
    numberOfChildren = argc - 1; // Number of inputs equals number of children to create
    // Create all children and run them all at once
    for (i = 0; i < numberOfChildren; i++) // Go through each command-line Fibonacci input
    {
        fibonacciInputs[i] = atoi(argv[i + 1]); // Convert the argument from a string to an integer
        if (fibonacciInputs[i] < 0) // Check that the Fibonacci input is not negative
        {
            printf("Error: Fibonacci input must be non-negative.\n"); // Print error for invalid input
            return 1; // End program if input is invalid
        }
        createChildProcess(fibonacciInputs[i]); // Create a child to compute this Fibonacci number
    }
    // Wait for all child processes to finish
    waitForChildren(numberOfChildren); // Parent waits until every child process is finished
    // Read and print the result from each child process
    for (i = 0; i < numberOfChildren; i++) // Go through each child's pipe
    {
        read(pipes[i][0], &result, sizeof(result)); // Read the Fibonacci result sent by the child
        close(pipes[i][0]); // Close the read end of the pipe after receiving the result
        printf(
            "Child Process (PID %d) F{%d} = %d\n", // Print the child PID, input, and Fibonacci result
            childPIDs[i], // PID of the child process
            fibonacciInputs[i], // Fibonacci input handled by this child
            result // Fibonacci result returned by the child
        );
    }
    return 0; // End the program successfully
}