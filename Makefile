all:
	@clang32 main.cpp lexer.cpp parser.cpp -o oca.exe

clean:
	@rm oca.exe
