bin/hlang-interpreter:	src/hlang-lexer.l src/hlang-parser.y
	bison --defines=headers/hlang-parser.h --output=build/hlang-parser.c src/hlang-parser.y
	flex -o build/hlang-lexer.c src/hlang-lexer.l
	cc -o bin/hlang-interpreter.out build/hlang-parser.c build/hlang-lexer.c -lfl
