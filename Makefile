all:
	@clang32 main.cpp lexer.cpp parser.cpp evaluator.cpp object.cpp memory.cpp -o oca.exe

clean:
	@rm oca.exe
