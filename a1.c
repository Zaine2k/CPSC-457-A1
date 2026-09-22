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

    // Maybe use an iterative approach to calcualte the fibonacci
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
    // Create child process, assignment wants in parallel so we fork the child processes here
    // Check if the fork fails
    // If not, proceed and call the fibonacci function and have the child output its result
    // Exit to prevent the child from creating more processes
}

// Function that waits for all child processes to finish
void waitForChildren(int numberOfChildren)
{
    // Run for all child processes that were made
    // Once children are all done processing 
    // Then parent process can stop, using wait() or waitpid()
}


int main(int argc, char *argv[])
{
    // Take arguments
    // Create all children and run them all at once
    return 0;
}