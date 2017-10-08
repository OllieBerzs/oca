SOURCES := main.cpp lexer.cpp parser.cpp evaluator.cpp object.cpp memory.cpp
OBJECTS := $(patsubst %,temp/%,$(SOURCES:.cpp=.o))

all: temp oca.exe

oca.exe: $(OBJECTS)
	@echo [Link]
	@clang32 -O2 -o oca.exe $^

temp/%.o: ./%.cpp
	@echo [Compile] $<
	@clang32 -O2 -MMD -MP -c -o $@ $<

temp:
	@mkdir -p temp

clean:
	@rm -rf temp
	@rm oca.exe

remake: clean all

.PHONY: all remake clean

-include $(OBJECTS:.o=.d)
