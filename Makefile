CVER = g++
WFLAGS = -Wall -Werror -ansi -pedantic
ODIR = bin

all: rshell

rshell: ; mkdir -p $(ODIR) ; $(CVER) src/main.cpp src/Command.cpp src/execute.cpp $(WFLAGS) -o $(ODIR)/rshell

clean: ; rm -rf bin
