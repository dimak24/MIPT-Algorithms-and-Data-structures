#pragma once

template<class RandomAccessIterator>
struct Run {
    RandomAccessIterator begin, end;

    Run(RandomAccessIterator beg, RandomAccessIterator en) : begin(beg), end(en) {}

    Run() = default;

    bool operator<(const Run<RandomAccessIterator>& run2) const {
        return *begin < *run2.begin || (*begin == *(run2.begin) && *(end - 1) <= *(run2.end - 1));
    }

    size_t len() const {
        return end - begin;
    }
};