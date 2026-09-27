# huffman-file-compressor

A lossless file compressor written in C++17 that uses Huffman coding.

- Counts byte frequencies, then builds the code tree with a priority queue
- Object-oriented design: `Huffman` codec plus `BitWriter` / `BitReader` streams
- Bit-level binary I/O with a compact header (only the symbols that appear are stored)

## Build

```sh
make            # or: g++ -std=c++17 -O2 -o huff main.cpp huffman.cpp
```

## Usage

```sh
./huff c input.txt input.huf     # compress
./huff d input.huf restored.txt  # decompress
```

Text files usually shrink by 40–60%. Files that are already compressed (zip, jpg) won't get smaller.
