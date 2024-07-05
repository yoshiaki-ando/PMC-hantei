OBJS = err_func.o main.o pmc_data.o
HEADERS = hantei.h observation_data.h

OPTS = -Wall -O3 -std=c++17 -I/home/ando/include
LIBS = -lnlopt

all: main

main: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS)

%.o: %.cpp $(HEADERS)
	g++ -c $< $(OPTS)
