#include "binary_heap.h"


template <typename T>
BinaryHeap<T>::BinaryHeap() : size(0) {}

template <typename T>
void BinaryHeap<T>::push(int key, const T& value) {
    heap.push_back({key, value});
    pos_map[&value] = heap.size() - 1;
    siftUp(heap.size() - 1);
    ++size;
}

template <typename T>
typename BinaryHeap<T>::Element BinaryHeap<T>::pop() {
    if (empty()) throw std::runtime_error("Heap is empty");
    Element root = heap.front();
    heap[0] = heap.back();
    heap.pop_back();
    if (!heap.empty()) siftDown(0);
    --size;
    return root;
}

template <typename T>
void BinaryHeap<T>::decreaseKey(const T& value, int newKey) {
    auto pos = pos_map.find(&value);
    if (pos == pos_map.end()) throw std::runtime_error("Value not found in heap");

    int index = pos->second;
    if (newKey >= heap[index].first) throw std::runtime_error("New key is not smaller");
    heap[index].first = newKey;
    siftUp(index);
}

template <typename T>
bool BinaryHeap<T>::empty() const {
    return size == 0;
}

template <typename T>
void BinaryHeap<T>::siftUp(size_t index) {
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap[index].first >= heap[parent].first) break;
        std::swap(heap[index], heap[parent]);
        pos_map[&heap[index].second] = index;
        pos_map[&heap[parent].second] = parent;
        index = parent;
    }
}

template <typename T>
void BinaryHeap<T>::siftDown(size_t index) {
    size_t left = 2 * index + 1;
    size_t right = 2 * index + 2;
    size_t smallest = index;

    if (left < heap.size() && heap[left].first < heap[smallest].first) {
        smallest = left;
    }

    if (right < heap.size() && heap[right].first < heap[smallest].first) {
        smallest = right;
    }

    if (smallest != index) {
        std::swap(heap[index], heap[smallest]);
        pos_map[&heap[index].second] = index;
        pos_map[&heap[smallest].second] = smallest;
        siftDown(smallest);
    }
}
