# makefile for building oca

# ----- Change settings below to suit your environment -----

CXX = g++
CPPFLAGS = -Wall -std=c++17

# ----- End of user settings -------------------------------

BIN = oca
OBJ = main.o lexer.o parser.o evaluator.o errors.o

all: $(BIN).exe std
std: std.ocalib

# object files
%.o: %.cpp
	@echo [Compile] $<
	@$(CXX) $(CPPFLAGS) -c $<

# binaries
$(BIN).exe: $(OBJ)
	@echo [Link] $(BIN).exe
	@$(CXX) $(CPPFLAGS) -o $(BIN).exe $^

std.ocalib: lib/std.cpp
	@echo [Compile] std.cpp
	@$(CXX) $(CPPFLAGS) -c -o lib/std.o lib/std.cpp
	@echo [Link] std.ocalib
	@$(CXX) $(CPPFLAGS) -o std.ocalib -shared lib/std.o -Wl,--subsystem,windows #add -s in release

clean:
	@echo [Clean] $(BIN)
	@$(RM) $(BIN).exe
	@$(RM) $(OBJ)
	@$(RM) std.ocalib
	@$(RM) lib/*.o

# dependencies
main.o: oca.hpp
lexer.o: lexer.hpp
parser.o: parser.hpp errors.hpp
evaluator.o: evaluator.hpp errors.hpp
errors.o: errors.hpp
 