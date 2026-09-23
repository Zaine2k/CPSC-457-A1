/*
Name: Zaine Ancheta
UCID: 30214484
Tutorial: T14
TA: Zahra Arabi

Assignment 1
*/

/*
Heads Up!
Ran into compilation errors because the provided MINIX compiler did not always support "//" comments.
It was able to run at first, however it made debugging annoying due to all the comment warnings and then down the line, errors.

I used ChatGPT to convert the "//" comments in this file to C-style block "/*" comments to avoid these errors.
Refer to my second commit on Sep 22 in my GitHub repository, I prompted it to only change comments and not any logic.
I did not want to go line by line changing comments! I hope you understand and I believe this doesn't violate any academic policy.
*/

/*
Omar on discord in "t05-t11-14-zahra", suggested this line to enable Minix declarations to everyone in tutorial.
I added it for any future changes.
*/
#define _MINIX 1

#include <stdlib.h> /* Standard header file for C */
#include <stdio.h> /* Header file for printf function */
#include <sys/types.h> /* Header file for process IDs (PID) */
#include <unistd.h> /* Header file for fork(), getpid(), and getppid() */
#include <sys/wait.h> /* Header file for wait() and waitpid() */

/*Constants and exit codes*/
#define MAX_CHILDREN 8

#define EXIT_SUCCESS_CODE 0
#define EXIT_INPUT_ERROR 1
#define EXIT_PIPE_ERROR 2
#define EXIT_FORK_ERROR 3
#define EXIT_WAIT_ERROR 4


pid_t childPIDs[MAX_CHILDREN]; /* Stores the PID of each child process */
int pipes[MAX_CHILDREN][2]; /* Stores one pipe for each child: [0] read end, [1] write end */
int childrenCreated = 0; /* Tracks how many child processes have been created */

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

    /*
    Reference: https://stackoverflow.com/questions/35616033/understanding-pipe-function 
    Accessed Sept 20
    Used this discussion to understand that pipe() creates two file descriptors: 
    index 0 is used to read from the pipe and index 1 is used to write to the pipe.
    */

    /* Create pipe for this child process */
    if (pipe(pipes[childIndex]) < 0)
    {
        perror("pipe failed");
        exit(EXIT_PIPE_ERROR);
    }

    /*
    Reference, Zahra Arabi, "fork" slide 4.
    I used her code from slide 4 to develop a basic structure for creating the rest
    of the code in this function.
    More specifically, the structure for a basic fork, and then incorporating my existing fibonacci function.
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

        /*
        Reference: https://man.archlinux.org/man/close.2.en
        Accessed Sept 22
        Used this documentation to understand how close() closes a file descriptor.
        A small optimization to close both of the pipe before we end the program.
        */
        close(pipes[childIndex][0]);
        close(pipes[childIndex][1]);

        exit(EXIT_FORK_ERROR);
    }

    /*
    If not, proceed and call the fibonacci function and have
    the child output its result.

    Reference, https://man7.org/linux/man-pages/man2/write.2.html?
    Accessed Sept 20
    Early in planning, I had initially designed it with snprintf() in mind, meaning taking the fibonacci integer output as text.
    Instead, I had decided to take the integer outputs, take their binary form and read out its binary form as an integer by default.
    This simplifies the IPC code tremendously and avoids compiler incompatibilities.
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
        exit(EXIT_SUCCESS_CODE);
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
        if (waitpid(childPIDs[i], NULL, 0) < 0)
        {
            perror("waitpid failed");
            exit(EXIT_WAIT_ERROR);
        }
    }
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

    /* 
    Take arguments and perform input validation.
    */

    /* Check that at least one Fibonacci number was provided */
    if (argc < 2)
    {
        /* Show correct program usage */
        printf("Usage: %s <n1> <n2> ... <n8>\n", argv[0]);

        /* End program because no input was given */
        return EXIT_INPUT_ERROR;
    }

    /* Checks if more arguments were passed than required */
    if (argc > MAX_CHILDREN + 1)
    {
        /* Tell user they entered too many values */
        printf("Error: Maximum of 8 arguments allowed.\n");

        /* End program because input is invalid */
        return EXIT_INPUT_ERROR;
    }

    /* Number of inputs equals number of children to create */
    numberOfChildren = argc - 1;

    /* Convert and validate all command-line arguments first */
    for (i = 0; i < numberOfChildren; i++)
    {   
        /*
        Reference: https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
        Accessed Sept 21
        Wanted a simple way to convert argument strings into integers.
        Used this documentation to understand how atoi() converts a string into an integer.
        */
        fibonacciInputs[i] = atoi(argv[i + 1]);

        /* Check that the Fibonacci input is not negative */
        if (fibonacciInputs[i] < 0)
        {
            /* Print error for invalid input */
            printf("Error: Fibonacci input must be non-negative.\n");

            /* End program if input is invalid */
            return EXIT_INPUT_ERROR;
        }
    }

    /* Create all children after the inputs have been validated */
    for (i = 0; i < numberOfChildren; i++)
    {
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

    return EXIT_SUCCESS_CODE;
}