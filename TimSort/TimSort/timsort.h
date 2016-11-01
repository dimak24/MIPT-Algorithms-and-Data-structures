#pragma once

#include <iostream>
#include <cmath>
#include "Run.h"
#include "runsStack.h"
#include "inplaceMerge.h"
#include "supportingSorts.h"
#include "supportingFunctions.h"
#include "ITimSortParams.h"

template<class RandomAccessIterator>
void separate_into_runs(const RandomAccessIterator& first, const RandomAccessIterator& last, 
                    RunsStack<RandomAccessIterator>* stack, 
                    const ITimSortParams& params) {

    size_t minrun = params.minRun(last - first);
    RandomAccessIterator it = first;
    RandomAccessIterator begin = first;

    while (it != last) {
        ++it;
        if (it == last && begin < last)
            stack->push(Run<RandomAccessIterator>(begin, it), params);
        else if (*it == *(it - 1))
            continue;
        else if (*it > *(it - 1)) {
            while (it != last && *it >= *(it - 1)) 
                ++it;
            if (it - begin >= minrun) 
                stack->push(Run<RandomAccessIterator>(begin, it), params);
            else {
                size_t d = min(last - it, minrun - (it - begin));
                it += d;
                InsertionSort(begin, it, it - begin - d);
                stack->push(Run<RandomAccessIterator>(begin, it), params);
            }
            begin = it;
        } else {
            while (it != last && *it <= *(it - 1)) 
                ++it;
            reverse_decreasing(begin, it);
            if (it - begin >= minrun)
                stack->push(Run<RandomAccessIterator>(begin, it), params);
            else {
                size_t d = min(last - it, minrun - (it - begin));
                it += d;
                InsertionSort(begin, it, it - begin - d);
                stack->push(Run<RandomAccessIterator>(begin, it), params);
            }
            begin = it;
        }
    }
}

template<class RandomAccessIterator, class Compare>
void separate_into_runs(const RandomAccessIterator& first, const RandomAccessIterator& last,
    RunsStack<RandomAccessIterator>* stack,
    Compare comp, 
    const ITimSortParams& params = DefaultParamsRealisation()) {

    size_t minrun = params.minRun(last - first);
    RandomAccessIterator it = first;
    RandomAccessIterator begin = first;

    while (it != last) {
        ++it;
        if (it == last && begin < last)
            stack->push(Run<RandomAccessIterator>(begin, it), comp, params);
        else if (!comp(*it, *(it - 1)) && !comp(*(it - 1), *it))
            continue;
        else if (comp(*(it - 1), *it)) {
            while (it != last && !comp(*it, *(it - 1)))
                ++it;
            if (it - begin >= minrun)
                stack->push(Run<RandomAccessIterator>(begin, it), comp, params);
            else {
                size_t d = min(last - it, minrun - (it - begin));
                it += d;
                InsertionSort(begin, it, comp, it - begin - d);
                stack->push(Run<RandomAccessIterator>(begin, it), comp, params);
            }
            begin = it;
        } else {
            while (it != last && !comp(*(it - 1), *it))
                ++it;
            reverse_decreasing(begin, it);
            if (it - begin >= minrun)
                stack->push(Run<RandomAccessIterator>(begin, it), comp, params);
            else {
                size_t d = min(last - it, minrun - (it - begin));
                it += d;
                InsertionSort(begin, it, comp, it - begin - d);
                stack->push(Run<RandomAccessIterator>(begin, it), comp, params);
            }
            begin = it;
        }
    }
}

template <class RandomAccessIterator>
void TimSort(const RandomAccessIterator& first, const RandomAccessIterator& last, 
        const ITimSortParams& params = DefaultParamsRealisation()) {

    RunsStack<RandomAccessIterator>* stack = new RunsStack<RandomAccessIterator>();

    separate_into_runs(first, last, stack, params);

    stack->finalMerge(params);
}

template <class RandomAccessIterator, class Compare>
void TimSort(const RandomAccessIterator& first, const RandomAccessIterator& last, Compare comp,
        const ITimSortParams& params = DefaultParamsRealisation()) {

    RunsStack<RandomAccessIterator>* stack = new RunsStack<RandomAccessIterator>();

    separate_into_runs(first, last, stack, comp, params);

    stack->finalMerge(comp, params);
}