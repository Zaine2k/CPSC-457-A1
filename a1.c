/*
Name: Zaine Ancheta
UCID: 30214484
Tutorial: T14
TA: Zahra Arabi

Assignment 1
*/

#include <stdlib.h> /* Standard header file for C */
#include <stdio.h> /* Header file for printf function */
#include <sys/types.h> /* Header file for process IDs (PID) */
#include <unistd.h> /* Header file for fork(), getpid(), and getppid() */
#include <sys/wait.h> /* Header file for wait() and waitpid() */

#define MAX_CHILDREN 8

pid_t childPIDs[MAX_CHILDREN];
int pipes[MAX_CHILDREN][2];
int childrenCreated = 0;

/* Function that computes to the nth Fibonacci number */
int fibonacci(int n)
{
    int previous;
    int current;
    int next;
    int i;

    /* Base case for Fibonacci sequence */
    if (n == 0)
    {
        return 0;
    }

    if (n == 1)
    {
        return 1;
    }

    /* Iterative approach for the fibonacci sequence */
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

/* Function that creates a child process to compute a Fibonacci number */
void createChildProcess(int n)
{
    pid_t childPID;
    int childIndex;
    int result;

    childIndex = childrenCreated;

    /* Create pipe for this child process */
    if (pipe(pipes[childIndex]) < 0)
    {
        perror("pipe failed");
        exit(1);
    }

    /*
    Reference, Zahra Arabi, "fork" slide 4. (Lines 72-96)
    I used her code from slide 4 to develop structure for creating the rest
    of the code in this function.
    More specifically, the structure for a basic fork, and then
    incorporating the fibonacci function.
    */

    /*
    Create child process, assignment wants in parallel so we fork
    the child processes here
    */
    childPID = fork();

    /* Check if the fork fails */
    if (childPID < 0)
    {
        perror("fork failed");
        exit(1);
    }

    /*
    If not, proceed and call the fibonacci function and have
    the child output its result
    */
    if (childPID == 0)
    {
        /* Child does not need to read from the pipe */
        close(pipes[childIndex][0]);

        result = fibonacci(n);

        /* Send the result to the parent process through the pipe */
        write(pipes[childIndex][1], &result, sizeof(result));

        /* Close the writing end of the pipe */
        close(pipes[childIndex][1]);

        /* Exit to prevent the child from creating more processes */
        exit(0);
    }

    /* Save the child process ID */
    childPIDs[childIndex] = childPID;

    /* Parent does not need to write to the pipe */
    close(pipes[childIndex][1]);

    childrenCreated++;
}

/* Function that waits for all child processes to finish */
void waitForChildren(int numberOfChildren)
{
    int i;

    /* Run for all child processes that were made */
    for (i = 0; i < numberOfChildren; i++)
    {
        /* Once children are all done processing */
        waitpid(childPIDs[i], NULL, 0);
    }

    /* Then parent process can stop, using wait() or waitpid() */
}

/*
Main function, argc is number of command-line arguments,
argv stores them
*/
int main(int argc, char *argv[])
{
    int numberOfChildren;
    int fibonacciInputs[MAX_CHILDREN];
    int result;
    int i;

    /* Take arguments */

    /* Check that at least one Fibonacci number was provided */
    if (argc < 2)
    {
        /* Show correct program usage */
        printf("Usage: %s <n1> <n2> ... <n8>\n", argv[0]);

        /* End program because no input was given */
        return 1;
    }

    /* Checks if more arguments were passed than required */
    if (argc > 9)
    {
        /* Tell user they entered too many values */
        printf("Error: Maximum of 8 arguments allowed.\n");

        /* End program because input is invalid */
        return 1;
    }

    /* Number of inputs equals number of children to create */
    numberOfChildren = argc - 1;

    /* Create all children and run them all at once */
    for (i = 0; i < numberOfChildren; i++)
    {
        /* Convert the argument from a string to an integer */
        fibonacciInputs[i] = atoi(argv[i + 1]);

        /* Check that the Fibonacci input is not negative */
        if (fibonacciInputs[i] < 0)
        {
            /* Print error for invalid input */
            printf("Error: Fibonacci input must be non-negative.\n");

            /* End program if input is invalid */
            return 1;
        }

        /* Create a child to compute this Fibonacci number */
        createChildProcess(fibonacciInputs[i]);
    }

    /* Wait for all child processes to finish */
    waitForChildren(numberOfChildren);

    /* Read and print the result from each child process */
    for (i = 0; i < numberOfChildren; i++)
    {
        /* Read the Fibonacci result sent by the child */
        read(pipes[i][0], &result, sizeof(result));

        /* Close the read end of the pipe after receiving the result */
        close(pipes[i][0]);

        /* Print the child PID, input, and Fibonacci result */
        printf(
            "Child Process (PID %d) F{%d} = %d\n",
            childPIDs[i],
            fibonacciInputs[i],
            result
        );
    }

    /* End the program successfully */
    return 0;
}