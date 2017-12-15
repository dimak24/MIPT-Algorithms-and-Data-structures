#pragma once

#include "Run.h"
#include "supportingFunctions.h"

template <class RandomAccessIterator>
void InsertionSort(RandomAccessIterator first, RandomAccessIterator last, const size_t& d = 0) {
        for (RandomAccessIterator i = first + d; i != last; ++i) {
            for (RandomAccessIterator j = i; j != first && *(j - 1) > *j; --j)
                std::swap(*j, *(j - 1));
        }
}

template <class RandomAccessIterator, class Compare>
void InsertionSort(RandomAccessIterator first, RandomAccessIterator last, Compare comp, const size_t& d = 0) {
    for (RandomAccessIterator i = first + d; i != last; ++i) {
        for (RandomAccessIterator j = i; j != first && comp(*j, *(j - 1)); --j)
            std::swap(*j, *(j - 1));
    }
}