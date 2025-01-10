# CppND-System-Monitor

## Overview

The Linux System Monitor is a terminal-based application designed to provide real-time system performance metrics, inspired by H-top. This lightweight and efficient application will enable users to monitor key system resources and processes directly from the command line. Built in C++, the tool leverages system libraries to gather and display information in an organized and user-friendly interface.

Linux System Monitor Project in the Object Oriented Programming Course of the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213).

![System Monitor](images/monitor.png)

## ncurses

[ncurses](https://www.gnu.org/software/ncurses/) is a library that facilitates text-based graphical output in the terminal. This project relies on ncurses for display output.

If you are not using the Workspace, install ncurses within your own Linux environment: `sudo apt install libncurses5-dev libncursesw5-dev`

## Make

This project uses [Make](https://www.gnu.org/software/make/). The Makefile has four targets:

- `build` compiles the source code and generates an executable
- `format` applies [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to style the source code
- `debug` compiles the source code and generates an executable, including debugging symbols
- `clean` deletes the `build/` directory, including all of the build artifacts

## Instructions

1. Build the project: `make build`

2. Run the resulting executable: `./build/monitor`
   ![Starting System Monitor](images/starting_monitor.png)
