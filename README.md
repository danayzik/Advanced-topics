# Advanced Topics Ex3 - Build Instructions

## Viewing the README

To view this README properly in the terminal, you should use the `less` command:
```bash

less README.md
```
To exit 'less'
```bash

q
```

## This project is submitted by


Dan Ayzik 206038929

Noam Barlin 314620071


## Requirements

CMake version 3.28 or higher is required.

C++20 or higher (equivalent to a recent version of the g++ compiler).


## Note
The compiler flags added in CMakeLists.txt are the required flags and are equivalent to:

```bash

g++ -std=c++20 -Wall -Wextra -Werror -pedantic
```

## General Instructions
Open a terminal and navigate to the project's directory:

```bash

cd /path/to/206038929_314620071
```

Build the project for Linux. Then run the project based on the "Run instructions".
If you're on windows, and you're having trouble running it, contact me for help.

## Linux Build Instructions

### Two configurations are available:
1) Building without logging:
```bash

make
```

2) Building with logging:
```bash

make log
```
You can also build each module by going into the appropriate directory and running the commands there.  
Note that the logging option only applies to the Simulator executable.  

### A note about logging:
Building the project with logging enabled, will cause many messages to flood the console, and will slow down  
the program.
We recommend you redirect the output to a file instead of the console by concatenating > log.txt  
To the usual run command.

## Error Documentation
### Unrecoverable errors:
1) Multiple mode arguments. e.g., both -competition and -comparative are present.
2) Repeating non-optional arguments
3) Missing a non-optional argument
4) Unexpected arguments: For a given mode, any argument that is not a mandatory/optional argument for that mode.
5) Invalid value for an argument, e.g. non-integral num_threads value
6) Invalid file/folder path given as an argument
7) Invalid/missing map settings for the map file in Comparative mode (These are case-sensitive, beware!)
8) One or more invalid Algorithm shared object or registration in Comparative mode
9) Less than 2 valid Algorithms for Competition mode
10) Less than 1 valid map for Competition mode
11) Invalid GameManager for Competition mode
12) Any segmentation fault or other **hardware** based exception that happens inside the loaded shared objects

### Recovered errors:
1) Repeating optional arguments, in the case of num_threads, last argument will apply
2) Out of order arguments (They can appear in any order as requested)
3) Duplicate registrations. If a module tries to register multiple times, it will be ignored
4) Unexpected characters in the map file map data section are ignored and filled as spaces
5) Missing rows/columns in the map data, compared to the declared settings. Any missing will all be filled with spaces
6) Out of order map settings, they can be written in any order, as long as they are the 4 lines that come after the  
    map's name.
7) Non-mandatory bad shared objects that won't load are skipped
8) Non-mandatory shared objects that failed to register correctly are skipped
9) Games that crashed due to a non-hardware exception are skipped, recovered and documented
10) An invalid game result from a game is also recovered and documented

**All recovered errors will be documented in an input_errors.txt file created in the running directory.**  
**Unrecoverable errors will be printed to std::cerr and the program will finish gracefully (apart from hardware exceptions)**


## Threading Model

### Notes:
**In both modes, the main thread also participates in running the simulated games**  
**Shared resource access like the storage of game results, or writing to the error buffer are locked**

### Comparative mode
In this mode we have 'n' Game Manager factories saved in the registrar, thus if a thread chooses an index i, 0 <= i < n  
He can take ownership and run that Game Manager.  
For this, we simply use one std::atomic<size_t> variable.  
When a thread is free, he atomically fetches the value of said index, increments it. If it's >= n, he finished his job.  
Else, he runs the appropriate Game Manager.  

### Competition mode
Similarly to comparative mode, we fetch the indices of the map we need to run and the algorithm that will act as player 1  
When fetched, we increment only the algorithm index, unless we reach 'n' (The number of algorithms)  
Then we set that index to 0 and increment the map index.  
If the map index >= k (The number of valid maps), the thread breaks.  
Since this is not a single atomic variable, we do use a dedicated mutex.

