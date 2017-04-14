bin/hlang-interpreter:	src/hlang-lexer.l src/hlang-parser.y
	@echo "+------------------------------------+"
	@echo "|  MAKE: HLang Interpreter Builder   |"
	@echo "+------------------------------------+"
	bison --defines=headers/buildtime_hlang-parser.h --output=build/hlang-parser.c src/hlang-parser.y --language=c
	flex --header-file=headers/buildtime_hlang-lexer.h -o build/hlang-lexer.c src/hlang-lexer.l
	gcc -o $@ build/hlang-parser.c build/hlang-lexer.c -lfl -I headers/ -std=gnu99

.PHONY:	clean reset
clean:	
	@echo "+--Cleaning clutter------------------+"
	-@rm -r build/* headers/buildtime* bin/*
	$(MAKE)
reset:
	@echo "+--Reset to bare sources------------+"
	@echo "Any errors (if any) suggesting files not found are okay"
	@-rm -r build/* headers/buildtime* bin/* 
