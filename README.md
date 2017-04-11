HLang
=====

HLang is a scripting language made primarily for becoming the main shell scripting language for [HelenOS](www.helenos.org). Currently the system is under development, with only primitive lexing capabilities.

----------

Building HLang from source
----------------
The HLang interpreter just features a primitive lexer at the present moment with no make possibilities. Hence, the compiling process is manual. To build HLang from source, download the code from the repository to a local machine, `cd` to the codebase and use the following commands:

To build the lexer:

    flex -o build/lexer.c src/hlang-lexer.l
    cc -o bin/hlang-lexer.out build/lexer.c -lfl
    
To run the lexical tests

    ./bin/hlang-lexer.out
