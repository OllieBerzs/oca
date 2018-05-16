# makefile for building oca

CXX = clang++
ARCH = x86_64
DEBUG = true

# End of user config -------------------------------------

ifeq ($(OS),Windows_NT)
BIN = oca.exe
TEST = tests.exe
TARGET = $(ARCH)-windows-gnu
else
BIN = oca
TEST = tests
TARGET = $(ARCH)-linux-gnu
endif

CPPFLAGS = -target $(TARGET) -Wall -std=c++17
ifeq ($(CXX), g++)
CPPFLAGS = -Wall -std=c++17
endif

ifeq ($(DEBUG), true)
CPPFLAGS += -g -O0
else
CPPFLAGS += -O2
endif

# Objects
BINOBJ = main.o
TESTOBJ = tests.o
OBJ = oca.o lex.o parse.o value.o scope.o eval.o error.o

all: $(BIN)

# object files
%.o:
	@echo [Compile] $<
	@$(CXX) $(CPPFLAGS) -c -o $@ $<

# binaries
$(BIN): $(BINOBJ) $(OBJ)
	@echo [Link] $(BIN)
	@$(CXX) $(CPPFLAGS) -o $(BIN) $^
	@$(RM) *.o-*

$(TEST): $(TESTOBJ) $(OBJ)
	@echo [Link] $(TEST)
	@$(CXX) $(CPPFLAGS) -o $(TEST) $^
	@$(RM) *.o-*

check:
	@cppcheck --enable=all --force $(OBJ:.o=.cpp) $(BINOBJ:.o=.cpp)

deps:
	@echo [Gen dependencies]
	@$(CXX) $(CPPFLAGS) -MM $(OBJ:.o=.cpp) $(BINOBJ:.o=.cpp) $(TESTOBJ:.o=.cpp) >> makefile

clean:
	@echo [Clean]
	@$(RM) $(BIN) $(TEST)
	@$(RM) *.o *.o-*

script: $(BIN)
	@echo [Run] script.oca
	@./$(BIN) examples/script.oca

repl: $(BIN)
	@echo [Repl]
	@./$(BIN)

test: $(TEST)
	@echo [Test]
	@./$(TEST)

.PHONY: test script clean deps all

# dependencies (generated) -----------------------------------
oca.o: oca.cpp oca.hpp common.hpp ocaconf.hpp lex.hpp scope.hpp value.hpp \
  parse.hpp eval.hpp error.hpp utils.hpp
lex.o: lex.cpp oca.hpp common.hpp ocaconf.hpp lex.hpp scope.hpp value.hpp \
  parse.hpp eval.hpp error.hpp
parse.o: parse.cpp oca.hpp common.hpp ocaconf.hpp lex.hpp scope.hpp \
  value.hpp parse.hpp eval.hpp error.hpp
value.o: value.cpp oca.hpp common.hpp ocaconf.hpp lex.hpp scope.hpp \
  value.hpp parse.hpp eval.hpp error.hpp utils.hpp
scope.o: scope.cpp oca.hpp common.hpp ocaconf.hpp lex.hpp scope.hpp \
  value.hpp parse.hpp eval.hpp error.hpp
eval.o: eval.cpp eval.hpp common.hpp ocaconf.hpp parse.hpp value.hpp \
  scope.hpp oca.hpp lex.hpp error.hpp
error.o: error.cpp error.hpp common.hpp ocaconf.hpp oca.hpp lex.hpp \
  scope.hpp value.hpp parse.hpp eval.hpp utils.hpp
main.o: main.cpp oca.hpp common.hpp ocaconf.hpp lex.hpp scope.hpp \
  value.hpp parse.hpp eval.hpp error.hpp
tests.o: tests.cpp catch2/catch.hpp oca.hpp common.hpp ocaconf.hpp \
  lex.hpp scope.hpp value.hpp parse.hpp eval.hpp error.hpp
