#pragma once

#include "Run.h"
#include "supportingSorts.h"
#include "ITimSortParams.h"

template <class RandomAccessIterator>
void inplaceMerge(RandomAccessIterator first, RandomAccessIterator middle,
    RandomAccessIterator last, const ITimSortParams& params) {

    size_t all = (last - first), size = sqrt(all), k = all / size;
    RandomAccessIterator x = first + size * size_t((middle - first + size - 1) / size - 1), buffer = first + (k - 1) * size;

    if (x < buffer)
        for (size_t i = 0; i < size; ++i)
            std::swap(*(x + i), *(buffer + i));

    RandomAccessIterator it_min;
    for (RandomAccessIterator i = first; i < buffer - size; i += size) {
        it_min = i;
        for (RandomAccessIterator j = i + size; j < buffer; j += size)
            if (*j < *it_min || (*j == *it_min && *(j + size - 1) < *(it_min + size - 1)))
                it_min = j;
        for (size_t j = 0; j < size; ++j)
            std::swap(*(i + j), *(it_min + j));
    }

    for (RandomAccessIterator i = first; i < buffer - size; i += size)
        _merge(i, buffer, size, params);

    size = last - buffer;
    InsertionSort(buffer - size, buffer + size);

    for (RandomAccessIterator i = buffer; i >= first + 2 * size; i -= size)
        _merge(i - 2 * size, buffer, size, params);

    InsertionSort(first, first + 2 * size);
    InsertionSort(buffer, buffer + size);
}

template <class RandomAccessIterator, class Compare>
void inplaceMerge(RandomAccessIterator first, RandomAccessIterator middle,
    RandomAccessIterator last, Compare comp, const ITimSortParams& params) {

    size_t all = (last - first), size = sqrt(all), k = all / size;
    RandomAccessIterator x = first + size * size_t((middle - first + size - 1) / size - 1), buffer = first + (k - 1) * size;

    if(x < buffer)
        for (size_t i = 0; i < size; ++i)
            std::swap(*(x + i), *(buffer + i));

    RandomAccessIterator it_min;
    for (RandomAccessIterator i = first; i < buffer - size; i += size) {
        it_min = i;
        for (RandomAccessIterator j = i + size; j != buffer; j += size)
            if (comp(*j, *it_min) || (!comp(*it_min, *j) && comp(*(j + size - 1), *(it_min + size - 1))))
                it_min = j;
        for (size_t j = 0; j < size; ++j)
            std::swap(*(i + j), *(it_min + j));
    }

    for (RandomAccessIterator i = first; i != buffer - size; i += size)
        _merge(i, buffer, size, comp, params);

    size = last - buffer;
    InsertionSort(buffer - size, buffer + size, comp);

    for (RandomAccessIterator i = buffer; i >= first + 2 * size; i -= size)
        _merge(i - 2 * size, buffer, size, comp, params);

    InsertionSort(first, first + 2 * size, comp);
    InsertionSort(buffer, buffer + size, comp);
}

template <class RandomAccessIterator>
void _merge(const RandomAccessIterator& first, const RandomAccessIterator& buffer, const size_t size, const ITimSortParams& params) {

    size_t gallop = params.getGallop();
    size_t from1 = 0, from2 = 0;

    for (size_t j = 0; j < size; ++j)
        std::swap(*(first + j), *(buffer + j));

    RandomAccessIterator res = first, p1 = buffer, p2 = first + size, last = p2 + size, buffer_end = buffer + size;

    while (res < last && p1 < buffer_end && p2 < last) {
        if (*p1 <= *p2) {
            std::swap(*(p1++), *(res++));
            ++from1;
        }
        else {
            std::swap(*(p2++), *(res++));
            ++from2;
        }

        if (from1 == gallop) {
            RandomAccessIterator left = p1, right = buffer_end, m;
            while (left < right - 1) {
                m = left + (right - left) / 2;
                (*m <= *p2 ? left : right) = m;
            }
            if (left < buffer_end && *left <= *p2)
                while (p1 <= left)
                    std::swap(*(p1++), *(res++));
            from1 = 0;
            from2 = 0;
        }
        else if (from2 == gallop) {
            RandomAccessIterator left = p2, right = last, m;
            while (left < right - 1) {
                m = left + (right - left) / 2;
                (*m < *p1 ? left : right) = m;
            }
            if (left < last && *left < *p1)
                while (p2 <= left)
                    std::swap(*(p2++), *(res++));
            from1 = 0;
            from2 = 0;
        }
    }

    while (p1 != buffer_end)
        std::swap(*(p1++), *(res++));

    while (p2 != last)
        std::swap(*(p2++), *(res++));
}

template <class RandomAccessIterator, class Compare>
void _merge(const RandomAccessIterator& first, const RandomAccessIterator& buffer, const size_t size, Compare comp, const ITimSortParams& params) {

    size_t gallop = params.getGallop();
    size_t from1 = 0, from2 = 0;

    for (size_t j = 0; j < size; ++j)
        std::swap(*(first + j), *(buffer + j));

    RandomAccessIterator res = first, p1 = buffer, p2 = first + size, last = p2 + size, buffer_end = buffer + size;

    while (res < last && p1 < buffer_end && p2 < last) {
        if (!comp(*p2, *p1)) {
            std::swap(*(p1++), *(res++));
            ++from1;
        }
        else {
            std::swap(*(p2++), *(res++));
            ++from2;
        }

        if (from1 == gallop) {
            RandomAccessIterator left = p1, right = buffer_end, m;
            while (left < right - 1) {
                m = left + (right - left) / 2;
                (!comp(*p2, *m) ? left : right) = m;
            }
            if (left < buffer_end && !comp(*p2, *left))
                while (p1 <= left)
                    std::swap(*(p1++), *(res++));
            from1 = 0;
            from2 = 0;
        }
        else if (from2 == gallop) {
            RandomAccessIterator left = p2, right = last, m;
            while (left < right - 1) {
                m = left + (right - left) / 2;
                (comp(*m, *p1) ? left : right) = m;
            }
            if (left < last && comp(*left, *p1))
                while (p2 <= left)
                    std::swap(*(p2++), *(res++));
            from1 = 0;
            from2 = 0;
        }
    }
    while (p1 != buffer_end)
        std::swap(*(p1++), *(res++));

    while (p2 != last)
        std::swap(*(p2++), *(res++));
}