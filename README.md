HLang
=====

HLang is a scripting language made primarily for becoming the main shell scripting language for [HelenOS](www.helenos.org). Currently the system is under development, with only primitive lexing and parsing capabilities.


Building HLang from source
----------------
The HLang interpreter is at present a set of lexer and analyser system. To build HLang from source, download the code from the repository to a local machine, `cd` to the codebase and use the following commands:

To build the system:

    make

To build the system, clean:

    make clean

To revert back to original sources:

    make reset

To run the interpreter

    ./run
