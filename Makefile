CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra

huff: main.cpp huffman.cpp huffman.hpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp huffman.cpp

clean:
	rm -f huff huff.exe
