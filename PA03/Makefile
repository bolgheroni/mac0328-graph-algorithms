BOOST_ROOT?=~/boost_1_80_0

SRCS=$(wildcard *.cpp)
DEPS=$(SRCS:.cpp=.d)
OBJS=$(SRCS:.cpp=.o)
PROG=main

INPUTS=$(sort $(wildcard tests/in*))
OUTS=$(INPUTS:tests/in%=tests/out%)

.SECONDARY: $(OUTS)

CXX=g++
CXXFLAGS=-Wall -pedantic -O2 -std=c++17 -I $(BOOST_ROOT)
RM=rm -fv

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
	$(RM) $(PROG) $(OBJS) $(DEPS) $(OUTS)

test: $(OUTS)

tests/out%: tests/in% $(PROG)
	-./$(PROG) < $< > $@ 2>&1

-include $(DEPS)
