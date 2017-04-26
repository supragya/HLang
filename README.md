HLang
=====


Current Prototype: **ANGRY AXOLOTL release 0 draft 4**

HLang is a scripting language made primarily for becoming the main shell scripting language for [HelenOS](http://www.helenos.org). Currently the system is under development, and can execute a very small subset of commands. For stable build versions, lookup the release page [here](https://www.github.com/supragya/HLang/releases). Still, USE WITH CAUTION - DO NOT DEPLOY.

What is HLang and what is this page?
----------------
HLang is a scripting language, whose interpreter's source code you have visited right now. The scripting language is not a general programming language, nor does it tries to clone any other available language - it is highly domain specific - for helping HelenOS users and developers write simple scipts for BDSH shell system so that various system operations can be done easily. This project tries to resolve the need as described here: [Ticket #653 Advanced shell scripting in Bdsh](http://www.helenos.org/ticket/653).


Building HLang from source
----------------
It is advised as above, if you want to evaluate a stable system, please go to the release page [here](https://www.github.com/supragya/HLang/releases). Still, if you want to build the current version from source, make sure you have the given tools:
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

**NOTE: IT IS HIGHLY ADVICED FOR TESTING, YOU LET ALL MODULES VERBOSE USING `./run alpves` **


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
For HLang, everything is a string be it `"hello"` or `20`. Both internally are stored as strings in what we call as `storagebin`. The storage bin is a hashmap structure with open addressing that allows for concept of scopes. Scopes allow variables to be allocated or deallocated as required braces open or close(This does differentiate between `keyvalpairs` decalaration of map variables. There is no global scope, at max there is a function scope. Any variables to be passed between functions, needs to be passed as an argument(see `executor.c`). This brings us to another concept: removal of function signature. Because everything is a string, and some functions may require multiple number of arguments, the signature does not make sense. Hence, any function definition does not have that, there is just names. **This is implemented**

### HLang libraries
The architecture of HLang execution is simplistic, it first reads the AST root node and tries finding the function to call (initially main). This can be tweaked to look beyond - to run commands written in C if the function the script is trying to call is not defined. For sake of simpliciy and segregation, these functioncall will have a prefix of `HL_`. For example, for finding the number of arguments, `HL_ARGLEN()` may be called, for string comparison or concatenation `HL_STCMP()` or `HL_STRCAT()` may be called. This allows for fast execution of functions and do not require AST traversal. **This is currently not implemented**

HLang constructs
----------------

HLang constructs are as follows (along with availability in AAd04):
* Sequential construct - General variable declarations (Implemented)
* Sequential construct - Map variable declarations (Implemented)
* Sequential construct - Variable assignments (Partially implemented)
* Sequential construct - Shellecho (Partially implemented)
* Sequential construct - Return (Not implemented - needs implementation of argument stack, WIP)
* Sequential construct - Functioncall (Partially implemented - does not pass or recieve arguments and returns)
* Selective construct  - IF-ELIF-ELSE(Implemented)
* Iterative construct  - while loop (Not implemented)
* Iterative construct  - for loop(Not implementd)

#### Sequential - General variable declarations
Sequential general variable decalration can declare and assign values to general variables (`$var`, `$roo`), and map element names(`$rr[5]`). For map element declaration, the map needs to be in the maplist of `vms` or else, the declaration tries adding it. The assignment can be done using another variable and/or simple expressions (although expressions currently do not currently work with decimal points and negatives, they will be added). The expression can include values returned by shellechos and functioncalls in them. 
Here is an example script snippet:

    declare $rr = $val, $var = 1+2*<% mkdir hlang >, $number1, $r[90];
    
A few things worth noting: the declaration is reverse (aka right to left). Hence, `$var` gets declared first and then `$rr`. Hence `$rr = $val` is valid. The shellecho `<% mkdir hlang >` is a string which actually was to be sent to the shell. This returns a return value which is plugged in here. This is not coupled with HelenOS BDSH at the present moment, hence a dummy executor asks in this case for a return value. (Presently any non zero return value is treated as 1, not the return value, needs correction there).

#### Sequential - Map variable declarations
A map variable can be declared either with assignments or without assignments. Both are as follows:

    declare map $nonassignedmap;
    declare map $assignedmap = {"key1" is 90, "key2" is "hello, "0" is 500};

It is worth noting that the expressions as values donot work currently with maps. This will be added soon. Also, iterating over a map needs full vms variable table lookup. TODO: Needs to save the binlocations of map elements with the map list.

#### Sequential - Variable assignments
While variable assignments does allow expressions, functioncall returns and shellechos for registering the assignment, at the current state, the system only is able to do: PREINCR, POSTINCR, PREDECR, POSTDECR as follows:

    $var++;
    ++$var;
    $var--;
    --$var;

Todo: expression addition etc.

#### Sequential - Shellecho
Shellechos are the way of HLang's talking to BDSH. This needs work, it does not substitute values in scenarios such as `<% cat {$filename} >`. This is one of the main concerns when it comes to going ahead. A substitution system is required for cases such as `$mapvariable[{$iteratorval}]` as well. Shellechos return execution status of the program: needs implementation to get the output data as well (will be easy through HLang library implementation)

#### Sequential - Return statements 
Not implemented - all functions return int - incorrect.

#### Sequential - Functioncalls
Functioncalls are permitted. However the return values cannot be taken out - argument stacks are not in place.

#### Selective - if-elif-else
HLang does nothing new here. Just an old fashioned condition based execution system. However, one thing worth noting is that HLang does not allow optional braces when it comes to single line executions after conditionals.

#### Iterative - while loop
Old fashioned while loop. Parsed and in AST, need to add structures for execution in `executor.c`

#### Iterative - for loop
Old fashioned for loop. Parsed and in AST, need to add structures for execution in `executor.c`
