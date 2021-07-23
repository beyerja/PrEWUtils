# PrEWUtils - Utensils to use PrEW with ease

This package provides some tools that allow for an easy usage of the `PrEW` package.

## Installation

<!-- TODO TODO TODO Comment on downloading module library -->
<!-- TODO TODO TODO DO INCLUDE module IN PrEWMultiSetupTest and PrEWRunExample -->
<!-- git clone -b 'v2.2' --depth 1 https://github.com/p-ranav/indicators.git -->

0. Make sure you installed `PrEW` (preferably in a directory next to this one).
 This is essential because `PrEWUtils` depends on both `PrEW` and the packages used within it.
1. Make sure your software is up-to-date (ROOT, gcc, cmake, ...) *or* load using macro (only on NAF):
 ```sh
 cd macros && source load_env.sh && cd ..
 ```
2. Compile the code:
 ```sh
 cd macros && chmod u+x compile.sh && ./compile.sh && cd ..
 ```
 The compilation can also be done in multithreaded mode using `./compile.sh --jobs=N_jobs`.
 
### Recompilation

If for some reason you want to change the source code and want to completely recompile this can be done with in the `macro` directory using `./compile.sh --rebuild [--jobs=N_jobs]`.

## Usage

The core idea of the package is to supply two kinds of classes: _setups_ and _runners_.

Setup classes provide an interface to tell PrEW the input and the instructions on how to form a proper fit with this input.

Runner classes provide an interface for performing the actual `PrEW` fit with a given setup.

### PrEWRunExample

The `PrEWRunExample` repository demonstrates how to use `PrEWUtils`.