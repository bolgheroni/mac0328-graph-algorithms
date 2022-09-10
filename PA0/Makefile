SRCS=$(wildcard *.cpp)
DEPS=$(SRCS:.cpp=.d)
OBJS=$(SRCS:.cpp=.o)
PROG=main

INPUTS=$(sort $(wildcard tests/in*))
SOLNS=$(INPUTS:tests/in%=tests/sol%)
OUTS=$(INPUTS:tests/in%=tests/out%)
DIFFS=$(INPUTS:tests/in%=tests/diff%)

.SECONDARY: $(OUTS)

CXX=g++
CXXFLAGS=-Wall -pedantic -O2 -std=c++17 -I ~/boost_1_80_0
RM=rm -fv
DIFF=diff

COMPILE.cpp = $(CXX) $(CXXFLAGS)
OUTPUT_OPTION=-MMD -MP -c
LINK.o = $(CXX) $(LDFLAGS)

all: $(PROG)

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(PROG) : $(OBJS)
	$(LINK.o) $^ -o $@

.PHONY: clean run test

run: $(PROG)
	./$(PROG)

clean:
	$(RM) $(PROG) $(OBJS) $(DEPS) $(OUTS) $(DIFFS)

test: $(DIFFS)

tests/out%: tests/in% $(PROG)
	./$(PROG) < $< > $@

tests/diff%: tests/out% tests/sol%
	-$(DIFF) $< $(<:tests/out%=tests/sol%) > $@
	-@[ -s $@ ] && echo "Differences found in $@" || true

-include $(DEPS)
