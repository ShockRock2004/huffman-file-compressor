#include "huffman.hpp"

#include <iterator>
#include <queue>
#include <vector>

namespace {
const char MAGIC[4] = {'H', 'U', 'F', '1'};

template <class T> void put(std::ostream& o, T v) { o.write(reinterpret_cast<const char*>(&v), sizeof v); }
template <class T> T get(std::istream& i) {
    T v{};
    if (!i.read(reinterpret_cast<char*>(&v), sizeof v)) throw std::runtime_error("bad header");
    return v;
}
}

std::unique_ptr<Huffman::Node> Huffman::buildTree(const Freq& freq) {
    auto cmp = [](Node* a, Node* b) { return a->freq != b->freq ? a->freq > b->freq : a->sym > b->sym; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> pq(cmp);
    for (int s = 0; s < 256; s++)
        if (freq[s]) pq.push(new Node{freq[s], s, nullptr, nullptr});
    if (pq.empty()) return nullptr;

    int next = 256;
    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        pq.push(new Node{a->freq + b->freq, next++, std::unique_ptr<Node>(a), std::unique_ptr<Node>(b)});
    }
    return std::unique_ptr<Node>(pq.top());
}

void Huffman::buildCodes(const Node* n, std::string& path, std::array<std::string, 256>& codes) {
    if (n->leaf()) { codes[n->sym] = path.empty() ? "0" : path; return; }
    path.push_back('0'); buildCodes(n->left.get(), path, codes); path.pop_back();
    path.push_back('1'); buildCodes(n->right.get(), path, codes); path.pop_back();
}

void Huffman::compress(std::istream& in, std::ostream& out) {
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    Freq freq{};
    for (unsigned char c : data) freq[c]++;

    uint16_t symbols = 0;
    for (auto f : freq) symbols += f != 0;

    out.write(MAGIC, 4);
    put<uint64_t>(out, data.size());
    put<uint16_t>(out, symbols);
    for (int s = 0; s < 256; s++)
        if (freq[s]) { put<uint8_t>(out, uint8_t(s)); put<uint64_t>(out, freq[s]); }

    auto root = buildTree(freq);
    if (!root) return;
    std::array<std::string, 256> codes;
    std::string path;
    buildCodes(root.get(), path, codes);

    BitWriter bw(out);
    for (unsigned char c : data)
        for (char bit : codes[c]) bw.write(bit == '1');
    bw.flush();
}

void Huffman::decompress(std::istream& in, std::ostream& out) {
    char magic[4];
    if (!in.read(magic, 4) || std::string(magic, 4) != std::string(MAGIC, 4)) throw std::runtime_error("not a .huf file");
    uint64_t size = get<uint64_t>(in);
    uint16_t symbols = get<uint16_t>(in);
    Freq freq{};
    for (int i = 0; i < symbols; i++) {
        uint8_t s = get<uint8_t>(in);
        freq[s] = get<uint64_t>(in);
    }

    auto root = buildTree(freq);
    BitReader br(in);
    for (uint64_t i = 0; i < size; i++) {
        const Node* n = root.get();
        if (n->leaf()) br.read();
        while (!n->leaf()) n = br.read() ? n->right.get() : n->left.get();
        out.put(char(n->sym));
    }
}
