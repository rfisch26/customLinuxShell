# Custom Linux Shell

A Unix shell built from scratch in C++, implementing core shell functionality including command parsing, process creation, and child process management using POSIX system calls.

## Features

- Execute standard Linux commands (e.g. `ls`, `pwd`, `cd`, `echo`)
- Fork/exec model for child process creation
- Custom `ChildProcess` class for process lifecycle management
- Graceful handling of invalid commands and error states
- Clean prompt loop with exit support

## How It Works

The shell runs a read-eval-print loop (REPL):

1. Displays a prompt and reads user input
2. Parses the command and arguments
3. Forks a child process using `fork()`
4. Executes the command in the child via `execvp()`
5. Parent process waits for the child to finish before prompting again

The `ChildProcess` class encapsulates the forking and execution logic, keeping the main shell loop clean and readable.

## Build & Run

**Requirements:** Linux or WSL, `g++` (C++11 or later)

```bash
g++ -o shell customLinuxShell.cpp ChildProcess.cpp
./shell
```

## Example

```
$ ./shell
> ls
ChildProcess.cpp  ChildProcess.h  customLinuxShell.cpp  README.md
> echo hello world
hello world
> exit
```

## What I Learned

Building this project gave me hands-on experience with:
- POSIX process management (`fork`, `execvp`, `waitpid`)
- Low-level C++ memory and string handling
- Separating concerns with class-based design in systems code
