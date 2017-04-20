test:	lex.l parse.y
	@echo "+------------------------------------+"
	@echo "|      MAKE: Sandbox Builder         |"
	@echo "+------------------------------------+"
	bison --defines=parse.h --output=parse.c parse.y
	flex --header-file=lex.h -o lex.c lex.l
	gcc -o $@ parse.c lex.c -lfl

.PHONY:	clean reset
clean:
	@echo "+--Cleaning clutter------------------+"
	-@rm simple lex.c lex.h parse.c parse.h
	$(MAKE)
