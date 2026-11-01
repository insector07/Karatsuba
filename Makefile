CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

all: bin/test bin/benchmark bin/calc

bin:
	mkdir -p bin

bin/test: tests/test_karatsuba.cpp src/bignum.hpp | bin
	$(CXX) $(CXXFLAGS) -o $@ tests/test_karatsuba.cpp

bin/benchmark: src/benchmark.cpp src/bignum.hpp | bin
	$(CXX) $(CXXFLAGS) -o $@ src/benchmark.cpp

bin/calc: src/calc.cpp src/bignum.hpp | bin
	$(CXX) $(CXXFLAGS) -o $@ src/calc.cpp

test: bin/test
	./bin/test

bench: bin/benchmark
	./bin/benchmark

clean:
	rm -rf bin
