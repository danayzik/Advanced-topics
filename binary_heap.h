#pragma once
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>

template <typename T>
class BinaryHeap {
public:
    using Element = std::pair<int, T>;
    BinaryHeap();
    Element pop();
    void decreaseKey(const T& value, int newKey);
    bool empty() const;
    void push(int key, const T& value);

private:
    std::vector<Element> heap;
    std::unordered_map<const T*, size_t> pos_map;
    size_t size;
    void siftUp(size_t index);
    void siftDown(size_t index);


};

