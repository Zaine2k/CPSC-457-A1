# CPSC 457 Assignment 1

This program creates child processes in MINIX to calculate Fibonacci numbers.

Each command-line argument represents a Fibonacci number to calculate. The program creates one child process for each input and allows the child processes to run in parallel.

The child processes send their calculated results back to the parent process using pipes.

## Setup (for CPSC 457)

In the Fedora VM, clone the repository into:

```bash
~/cpsc457-minix-boilerplate-0.2.3/home/src/a1
```

For example:

```bash
cd ~/cpsc457-minix-boilerplate-0.2.3/home/src
git clone https://github.com/Zaine2k/CPSC-457-A1.git a1
```

Then move to the boilerplate directory:

```bash
cd ~/cpsc457-minix-boilerplate-0.2.3
```

Copy the source files into MINIX:

```bash
./copy_files_src.sh
```

Start MINIX using the course-provided setup.

Inside MINIX, navigate to the assignment directory:

```bash
cd /home/a1
```

Compile the program:

```bash
cc a1.c -o a1
```

Run the program:

```bash
./a1 <n1> <n2> ... <n8>
```

Example for inputting:

```bash
./a1 3 5 2 9 20
```

Example output:

```text
Child Process (PID 123) F{3} = 2
Child Process (PID 124) F{5} = 5
Child Process (PID 125) F{2} = 1
Child Process (PID 126) F{9} = 34
Child Process (PID 127) F{20} = 6765
```

The process IDs will be different each time the program is run.

## Input Validation

If no arguments are provided, the program displays:

```text
Usage: ./a1 <n1> <n2> ... <n8>
```

If more than 8 inputs are provided, the program displays:

```text
Error: Maximum of 8 arguments allowed.
```

If a negative number is provided, the program displays:

```text
Error: Fibonacci input must be non-negative.
```
