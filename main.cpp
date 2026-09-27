#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "huffman.hpp"

int main(int argc, char** argv) {
    std::string mode = argc == 4 ? argv[1] : "";
    if (mode != "c" && mode != "d") {
        std::cerr << "usage: huff c|d <input> <output>\n";
        return 1;
    }

    std::ifstream in(argv[2], std::ios::binary);
    std::ofstream out(argv[3], std::ios::binary);
    if (!in || !out) {
        std::cerr << "cannot open files\n";
        return 1;
    }

    try {
        if (mode == "c") Huffman::compress(in, out);
        else Huffman::decompress(in, out);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    out.close();

    auto a = std::filesystem::file_size(argv[2]);
    auto b = std::filesystem::file_size(argv[3]);
    std::cout << a << " -> " << b << " bytes";
    if (mode == "c" && a) std::cout << " (" << 100.0 - 100.0 * double(b) / double(a) << "% smaller)";
    std::cout << "\n";
}
