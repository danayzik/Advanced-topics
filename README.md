# Advanced Topics Ex2 - Game Build Instructions

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

An internet connection is required to fetch the SFML library during the build process.

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

Build the project for Linux or for windows based on the following instructions. Then run the project based on the "Run instructions"

## Linux Build Instructions

### Two configurations are available:
1) Building without visuals:
```bash

make headless
```

2) Building with visuals:
```bash

make visuals
```

### Dependencies for make visuals

In order to build with visuals, some additional dependencies may need to be installed.

If you don't have them, it's recommended to run the following commands:

```bash

sudo apt-get update
sudo apt-get install libx11-dev libxrandr-dev libxcursor-dev libxi-dev
sudo apt-get install libgl1-mesa-dev
sudo apt install libx11-dev libxrandr-dev libxcursor-dev libxi-dev libgl1-mesa-dev libudev-dev
sudo apt install libfreetype6-dev
sudo apt install libvorbis-dev libogg-dev
sudo apt install libflac-dev
```

## Windows Build Instructions

On Windows, I did not create a make file. You need to manually run the following commands for building:

Remove previous build:
```bash

rd /s /q build
```

Build headless mode or visual mode:

Headless mode:
```bash

cmake -S . -B build -DUSE_SFML=OFF
```
Visual mode:
```bash

cmake -S . -B build -DUSE_SFML=ON
```

Build the project:
```bash

cmake --build build
```

## Run instructions

Linux:
```bash

./tanks_game <Map_File_Name>
```

Windows:
```bash

tanks_game.exe <Map_File_Name>
```
