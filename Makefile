SRC=$(wildcard *.cpp)
DST=$(patsubst %.cpp,%,$(SRC))

default: $(DST)

%: %.cpp
	g++ -std=c++14 -O3 -o $@ $^

clean:
	rm -rf $(DST)
