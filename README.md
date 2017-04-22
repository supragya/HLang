HLang
=====


WIP: **ANGRY AXOLOTL release 0 draft 4**

HLang is a scripting language made primarily for becoming the main shell scripting language for [HelenOS](http://www.helenos.org). Currently the system is under development, with only primitive lexing and parsing capabilities. Variable management subsystem is currently in place, however since the executor and ast generation is not fully done, nothing uses it. HLang is unstable and in many terms UNUSABLE at current stage. For stable versions, lookup the release page [here](https://www.github.com/supragya/HLang/releases). Still, USE WITH CAUTION - DO NOT DEPLOY.


Building HLang from source
----------------
The HLang interpreter is at present a set of lexer and analyser system. A bit of AST buildup is done, but it is far from ideal. To build HLang from source, download the code from the repository to a local machine using:

    git clone https://github.com/supragya/HLang.git HLang

You may change "HLang" at the end of command to whatever directory you want the codebase to be stored at. Then `cd` to the codebase (folder HLang here) and use the following commands:

To build the system:

    make

To build the system, clean:

    make clean

To revert back to original sources:

    make reset

To run the interpreter

    ./run <verbose-options>

Verbose options are options that you provide while executing the interpreter stating what modules should print the verbose outputs. Following are the letters corresponding to various modules:

    a :  Abstract Syntax tree
    l :  Lexer
    p :  Parser

For example if you want to run the interpreter with verbose output on screen from Abstract Syntax Tree, you may use: `./run a`. If you want to run the interpreter with verbose output from parser, you may use `./run p`. In case, verbose output from multiple modules is required, they can be stacked for that, in the following manner:

    ./run lpa

The above gives verbose output from all of lexer, parser and abstract syntax tree. Also it is worth noting that `./run lpa` is equivalent to `./run alp`. The order does not matter.
