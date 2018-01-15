# makefile for building oca

# ----- Change settings below to suit your environment -----

# testing
TESTSCRIPT = tests/script.oca

# compilation
CXX = g++
CPPFLAGS = -Wall -std=c++17

BINDIR = 
OBJDIR = temp

# ----- End of user settings -------------------------------

BIN = oca
OBJ = main.o lexer.o parser.o evaluator.o errors.o
#OBJ = main.o

all: $(BIN)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $<

# binary executable
$(BIN): $(OBJ)
	$(CXX) $(CPPFLAGS) -o $(BIN).exe $^

# run tests
run: $(BIN)
	$(BIN) $(TESTSCRIPT)

clean:
	$(RM) $(BIN)
	$(RM) $(OBJ)

# dependencies
main.o: oca.hpp
lexer.o: lexer.hpp
parser.o: parser.hpp errors.hpp
evaluator.o: evaluator.hpp errors.hpp
errors.o: errors.hpp
 