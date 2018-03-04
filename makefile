# makefile for building oca

CXX = clang++
ARCH = x86_64
DEBUG = true

# End of user config -------------------------------------

ifeq ($(OS),Windows_NT)
BIN = oca.exe
TEST = tester.exe
TARGET = $(ARCH)-windows-gnu
else
BIN = oca
TEST = tester
TARGET = $(ARCH)-linux-gnu
endif

ifeq ($(CXX), g++)
CPPFLAGS = -Wall -std=c++17
endif

ifeq ($(DEBUG), true)
CPPFLAGS += -g -O0
else
#CPPFLAGS += -O2
endif

# Objects
BIN_OBJ = main.o
TEST_OBJ = tests/test.o
OBJ = oca.o lex.o parse.o object.o scope.o eval.o

all: $(BIN)

# object files
%.o: %.cpp
	@echo [Compile] $<
	@$(CXX) $(CPPFLAGS) -c -o $@ $<

# binaries
$(BIN): $(BIN_OBJ) $(OBJ)
	@echo [Link] $(BIN)
	@$(CXX) $(CPPFLAGS) -o $(BIN) $^

$(TEST): $(TEST_OBJ) $(OBJ)
	@echo [Link] $(TEST)
	@$(CXX) $(CPPFLAGS) -o $(TEST) $^

clean:
	@$(RM) $(BIN) $(TEST)
	@$(RM) $(OBJ) $(TEST_OBJ) $(BIN_OBJ)
	@$(RM) *.ilk *.pdb

run: $(BIN)
	@./$(BIN) tests/script.oca

test: $(TEST)
	@./$(TEST)

# dependencies
OCA_H = oca.hpp common.hpp scope.hpp
LEX_H = lex.hpp common.hpp
PARSE_H = parse.hpp common.hpp
OBJECT_H = object.hpp common.hpp
SCOPE_H = scope.hpp common.hpp
EVAL_H = eval.hpp common.hpp

test.o: $(OCA_H) $(OBJECT_H)
main.o: $(OCA_H)
oca.o: $(OCA_H) $(LEX_H) $(PARSE_H) $(OBJECT_H) $(EVAL_H)
lex.o: $(LEX_H)
parse.o: $(PARSE_H) $(LEX_H)
object.o: $(OBJECT_H) $(PARSE_H)
scope.o: $(SCOPE_H) $(OBJECT_H)
eval.o: $(EVAL_H) $(PARSE_H) $(OBJECT_H)
