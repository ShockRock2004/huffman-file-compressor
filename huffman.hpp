#pragma once
#include <array>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

class BitWriter {
public:
    explicit BitWriter(std::ostream& out) : out_(out) {}
    void write(bool bit) {
        buf_ = uint8_t((buf_ << 1) | bit);
        if (++n_ == 8) flush();
    }
    void flush() {
        if (!n_) return;
        out_.put(char(buf_ << (8 - n_)));
        buf_ = 0; n_ = 0;
    }
private:
    std::ostream& out_;
    uint8_t buf_ = 0;
    int n_ = 0;
};

class BitReader {
public:
    explicit BitReader(std::istream& in) : in_(in) {}
    bool read() {
        if (!n_) {
            int c = in_.get();
            if (c == EOF) throw std::runtime_error("truncated input");
            buf_ = uint8_t(c); n_ = 8;
        }
        return (buf_ >> --n_) & 1;
    }
private:
    std::istream& in_;
    uint8_t buf_ = 0;
    int n_ = 0;
};

class Huffman {
public:
    static void compress(std::istream& in, std::ostream& out);
    static void decompress(std::istream& in, std::ostream& out);

private:
    using Freq = std::array<uint64_t, 256>;
    struct Node {
        uint64_t freq;
        int sym;
        std::unique_ptr<Node> left, right;
        bool leaf() const { return !left; }
    };
    static std::unique_ptr<Node> buildTree(const Freq& freq);
    static void buildCodes(const Node* n, std::string& path, std::array<std::string, 256>& codes);
};
