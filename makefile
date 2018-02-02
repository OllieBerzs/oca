# makefile for building oca

# ----- Change settings below to suit your environment -----

CXX = g++
CPPFLAGS = -Wall -std=c++17 -g

# ----- End of user settings -------------------------------

BIN = oca
OBJ = main.o lex.o parse.o value.o scope.o eval.o

all: $(BIN).exe

# object files
%.o: %.cpp
	@echo [Compile] $<
	@$(CXX) $(CPPFLAGS) -c $<

# binaries
$(BIN).exe: $(OBJ)
	@echo [Link] $(BIN).exe
	@$(CXX) $(CPPFLAGS) -o $(BIN).exe $^

clean:
	@echo [Clean] $(BIN)
	@$(RM) $(BIN).exe
	@$(RM) $(OBJ)

# dependencies
OCA_H = oca.hpp common.hpp lex.hpp parse.hpp scope.hpp
LEX_H = lex.hpp common.hpp
PARSE_H = parse.hpp common.hpp
VALUE_H = value.hpp common.hpp
SCOPE_H = scope.hpp common.hpp
EVAL_H = eval.hpp common.hpp

main.o: $(OCA_H)
lex.o: $(LEX_H)
parse.o: $(PARSE_H) $(LEX_H)
value.o: $(VALUE_H) $(PARSE_H)
scope.o: $(SCOPE_H) $(VALUE_H)
eval.o: $(EVAL_H) $(PARSE_H) $(VALUE_H)
 