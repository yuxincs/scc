# Scc 
[![Build Status](https://github.com/yuxincs/scc/workflows/build/badge.svg)](https://github.com/yuxincs/scc/actions)

A Simple C Compiler Written in C For Compiler Course Project.

It aims only to compile a subset of C Standard to MIPS Assembly.

The generated MIPS code can thus be run on any MIPS simulators or in real environment that supports MIPS. For testing purposes I run the code on [QtSPIM](http://spimsimulator.sourceforge.net/), a good multi-platform MIPS simulator.

## Current feature set
* Basic operations (e.g. addition multiplication less than operation bitwise negation etc.)
* Comments (`// foo` and `/* foo */`)
* Sequences of statements (`foo; bar`)
* Return statements
* If statements and else statements
* Local variables
* Basic varibale types (only `float` and `int` is supported)
* Variable assignment
* While loops (`while (foo) { bar }`)
* Function calls
* Binary expression
* Unary expression
* Only supports `int` type to be compiled, but provide syntax and semantic checks for all types
* No preprocessors
* No `break` statement

## Build

The project is managed by `CMake` and needs `flex` and `bison` installed to compile the code.

Simply run

```sh
mkdir build
cd build
cmake ..
make
```

And the compiler is ready to use. In the `tests` directory are some basic tests I personally use to test the compiler, including some with intentional syntax and semantic errors.

## Usage
To compile the source code using the compiler, run
```sh
./scc /path/to/source_file.c
```

And to enable debug output, run
```sh
./scc /path/to/source_file.c -i -s
```

Will output intermediate result such as intermediate instructions(`-i`) and syntax tree(`-s`).

 `-o` will set the path for the assembly code output.
 
## License
[MIT](https://github.com/yuxincs/scc/blob/master/LICENSE).
