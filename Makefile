SOURCES := main.cpp lexer.cpp parser.cpp evaluator.cpp object.cpp memory.cpp errors.cpp
OBJECTS := $(patsubst %,temp/%,$(SOURCES:.cpp=.o))

all: temp oca

oca: $(OBJECTS)
	@echo [Link]
	@clang++ -std=c++17 -Wall -O2 -o oca $^

temp/%.o: ./%.cpp
	@echo [Compile] $<
	@clang++ -std=c++17 -Wall -O2 -MMD -MP -c -o $@ $<

temp:
	@mkdir -p temp

clean:
	@rm -rf temp
	@rm oca

remake: clean all

.PHONY: all remake clean

-include $(OBJECTS:.o=.d)
