#pragma once

template<class RandomAccessIterator>
void reverse_decreasing(const RandomAccessIterator& first, const RandomAccessIterator& last) {
    size_t n = (last - first) / 2;
    for (size_t i = 0; i < n; ++i)
        std::swap(*(first + i), *(last - 1 - i));
}

size_t min(const size_t& a, const size_t& b) {
    return a <= b ? a : b;
}