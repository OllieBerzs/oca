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
#CPPFLAGS += -O2
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
	@$(RM) *.o-*

# binaries
$(BIN): $(BINOBJ) $(OBJ)
	@echo [Link] $(BIN)
	@$(CXX) $(CPPFLAGS) -o $(BIN) $^
	@$(RM) *.o-*

$(TEST): $(TESTOBJ) $(OBJ)
	@echo [Link] $(TEST)
	@$(CXX) $(CPPFLAGS) -o $(TEST) $^
	@$(RM) *.o-*

deps:
	@echo [Gen dependencies]
	@$(CXX) -MM $(OBJ:.o=.cpp) $(BINOBJ:.o=.cpp) $(TESTOBJ:.o=.cpp) >> makefile

clean:
	@echo [Clean]
	@$(RM) $(BIN) $(TEST)
	@$(RM) *.o *.o-*

script: $(BIN)
	@echo [Run] script.oca
	@./$(BIN) examples/script.oca

test: $(TEST)
	@echo [Test]
	@./$(TEST) -d yes

.PHONY: test script clean deps

# dependencies (generated) -----------------------------------
oca.o: oca.cpp oca.hpp common.hpp ocaconf.hpp scope.hpp value.hpp \
  parse.hpp eval.hpp lex.hpp error.hpp
lex.o: lex.cpp lex.hpp common.hpp ocaconf.hpp error.hpp value.hpp \
  scope.hpp
parse.o: parse.cpp parse.hpp common.hpp ocaconf.hpp lex.hpp error.hpp \
  value.hpp scope.hpp
value.o: value.cpp value.hpp common.hpp ocaconf.hpp scope.hpp parse.hpp \
  oca.hpp eval.hpp error.hpp
scope.o: scope.cpp scope.hpp common.hpp ocaconf.hpp value.hpp
eval.o: eval.cpp eval.hpp common.hpp ocaconf.hpp parse.hpp value.hpp \
  scope.hpp oca.hpp error.hpp
error.o: error.cpp error.hpp common.hpp ocaconf.hpp value.hpp scope.hpp \
  lex.hpp parse.hpp
main.o: main.cpp oca.hpp common.hpp ocaconf.hpp scope.hpp value.hpp \
  parse.hpp eval.hpp
tests.o: tests.cpp catch2/catch.hpp oca.hpp common.hpp ocaconf.hpp \
  scope.hpp value.hpp parse.hpp eval.hpp
