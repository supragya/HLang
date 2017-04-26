HLang
=====


Current Prototype: **ANGRY AXOLOTL release 0 draft 4**

HLang is a scripting language made primarily for becoming the main shell scripting language for [HelenOS](http://www.helenos.org). Currently the system is under development, and can execute a very small subset of commands. For stable build versions, lookup the release page [here](https://www.github.com/supragya/HLang/releases). Still, USE WITH CAUTION - DO NOT DEPLOY.

What is HLang and what is this page?
----------------
HLang is a scripting language, whose interpreter's source code you have visited right now. The scripting language is not a general programming language, nor does it tries to clone any other available language - it is highly domain specific - for helping HelenOS users and developers write simple scipts for BDSH shell system so that various system operations can be done easily. This project tries to resolve the need as described here: [Ticket #653 Advanced shell scripting in Bdsh](http://www.helenos.org/ticket/653).


Building HLang from source
----------------
It is adviced as above, if you want to evaluate a stable system, please go to the release page here. Still, if you want to build the current version from source, make sure you have the given tools:
1. bison (GNU Bison) 3.0.4
2. flex 2.6.0
3. gcc 5.4.0

It is not absolutely necessary that you may need the exact version numbers of the gives tools, however this is on what the source was build initially. Check your versions using `bison --version`, `flex --version` and `gcc version` commands. Once you have those, download the code from the repository to a local machine using:

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
    v :  Variable Management subsystem
    e :  Executor
    s :  Scope Management subsystem

For example if you want to run the interpreter with verbose output on screen from Abstract Syntax Tree, you may use: `./run a`. If you want to run the interpreter with verbose output from parser, you may use `./run p`. In case, verbose output from multiple modules is required, they can be stacked for that, in the following manner:

    ./run lpa

The above gives verbose output from all of lexer, parser and abstract syntax tree. Also it is worth noting that `./run lpa` is equivalent to `./run alp`. The order does not matter.


What is the directory structure? Where are the tests?
----------------
The directory structure of HLang is as provided. The `tests` lie in tests folder. You run the interpreter using `./run <verbose-options>` from the directory where both `tests` and `bin` folders are visible. Any testing (for AAd04 atleast) can be done using modifications in `tests/aux_test.hl`. If however, you want to test a script which is named differently, modify the same in `main` function of the HLang Interpreter.


What are HLang's capabilities?
----------------
The following section tries to give a overview of the capabilities that the language deems to provide. However it is worth noting that many, if not all of the given language capabilities are not currently in place right now (AAd04). However, correct architectural information resides in AST for making them work.

A simple HLang script may look like this:
        
        #!/script/HLang
        function main{
            declare map $n = {"key1" is 90, "key2" is 88, "key3" is "keys"};
            declare $var = 1+2*$n[key1];
            foo($var);
            while($var != 0){
                $var--;
            }
            if($var != 90){
                declare $root=func($nut), $nut = 99+$n[key2]; #right one eval first
                <% grep {$root} rndfile.c>;
            }
            else{
                <% echo "Cannot do" >
            }
            return 0;
        }
        function foo{
            <% grep {$var} rndfile.c >;
        }
        function func{
            return HL_STRCAT($1, ".0");
        }

Now, with the real part: is all this implemented? Theoritically yes, practically, need some more time. How much more time? Well the AST generation is done, and it is quite good - the executor needs to be build to execute the parse tree. At the current stage (AAd04), the loops are 4 hours worth of work to implement. However there is one or two grave issues that need to be addressed first (given below) before all implementation is done.

First, let's talk about where HLang sits when compared with BDSH shell system. The BDSH shell system will have a new directory `script` along with `cmds` in it's directory. The HLang system sits in `script/HLang` directory. BDSH will be modified to read `./file` commands and do the following:
1. Read the first line of "file"
2. Check if there is #!, if yes, check location of interpreter given ahead, and execute the interpreter, if no show error
3. Interpreter talks to BDSH using IPC calls

### After execution of HLang
As with any interpreter, the HLang interpreter tries reading and executing the file token by token. However, the AST generation is tightly liked with parsing - which is in sequence to execution before the script executor. It is worth considering: The interpreter creates full AST before executing the first line of the script. This has a drawback: larger amount of memory is required when AST is made fully than freed on line by line execution. However this has a few advantages too: Since AST generation is done first, there is no need to create a function declaration before calling the function, the parsing errors are handled before the executor error and one major thing (however optional): This allows AST to be saved on a file, compiling it source to source for recurring scripts. A minimal interpreter (stripped down version of HLang, only executor) may be deployed in low memory scenario.

### HLang variable management system
For HLang, everything is a string be it `"hello"` or `20`. Both internally are stored as string in what we call as `storagebibn`. The storage bin is a hashmap structure with open addressing that allows for concept of scopes. There is no global scope, at max there is a function scope. Any variables to be passed, needs to be passed as an argument. This brings us to another concept: removal of function signature. Because everything is a string, 
