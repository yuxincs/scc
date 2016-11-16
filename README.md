# Scc [![Build Status](https://travis-ci.org/RyanWangGit/scc.svg?branch=master)](https://travis-ci.org/RyanWangGit/scc)

A Simple C Compiler Written in C For Compiler Course Project.

It aims only to compile a subset of C Standard to MIPS Assembly.

The generated MIPS code can thus be run on any MIPS simulators or in real environment that supports MIPS. For test I run the code on QtSPIM, a good MIPS simulator for both Windows and Linux.

With syntax and semantic checks available, I aim to make the compiler as complete as possible though it is just a toy compiler.

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

## TODO
* Preprocessor usage
* Support for `break`

## License

MIT license.

## Build

The project is managed by `CMake` and needs `flex` and `bison` installed to compile the code.

Simply run

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

And the compiler is ready to use. In the `tests` directory are some basic tests I personally use to test the compiler,
run `make test` to run all the basic test source files, including some with intentional syntax and semantic errors.

## Usage
To compile the source code using the compiler, run
```
./scc /path/to/source_file.c
```

And to enable debug output, run
```
./scc /path/to/source_file.c -i -s
```

Will output intermediate result such as intermediate instructions(`-i`) and syntax tree(`-s`).

 `-o` will set the path for the assembly code output.
