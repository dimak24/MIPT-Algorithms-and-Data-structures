#pragma once

#include <cassert>
#include "Run.h"
#include "ITimSortParams.h"

template<class RandomAccessIterator>
class RunsStack {
private:
    const size_t _INIT_STACK_SIZE = 2000000;

    size_t _size;

    size_t* stack;

    size_t sum;

    RandomAccessIterator bottom;

    template <class Compare>
    void correct_stack(Compare comp, const ITimSortParams& params) {
        if (_size == 1)
            return;
        if (_size == 2) {
            if (params.needMerge(second_len(), first_len())) {
                inplaceMerge(bottom + sum - first_len() - second_len(), bottom + sum - first_len(), bottom + sum, comp, params);
                stack[_size - 2] += stack[_size - 1];
                --_size;
                return;
            }
            else
                return;
        }
        else {
            EWhatMerge WM = params.whatMerge(third_len(), second_len(), first_len());
            if (WM == WM_NoMerge)
                return;
            else if (WM == WM_MergeXY) {
                inplaceMerge(bottom + sum - first_len() - second_len() - third_len(),
                    bottom + sum - first_len() - second_len(), bottom + sum - first_len(), comp, params);
                stack[_size - 3] += stack[_size - 2];
                stack[_size - 2] = stack[_size - 1];
                --_size;
                correct_stack(comp, params);
            }
            else {
                inplaceMerge(bottom + sum - first_len() - second_len(), bottom + sum - first_len(), bottom + sum, comp, params);
                stack[_size - 2] += stack[_size - 1];
                --_size;
                correct_stack(comp, params);
            }
        }
    }

    void correct_stack(const ITimSortParams& params) {
        if (_size == 1)
            return;
        if (_size == 2) {
            if (params.needMerge(second_len(), first_len())) {
                inplaceMerge(bottom + sum - first_len() - second_len(), bottom + sum - first_len(), bottom + sum, params);
                stack[_size - 2] += stack[_size - 1];
                --_size;
                return;
            }
            else
                return;
        }
        else {
            EWhatMerge WM = params.whatMerge(third_len(), second_len(), first_len());
            if (WM == WM_NoMerge)
                return;
            else if (WM == WM_MergeXY) {
                inplaceMerge(bottom + sum - first_len() - second_len() - third_len(),
                    bottom + sum - first_len() - second_len(), bottom + sum - first_len(), params);
                stack[_size - 3] += stack[_size - 2];
                stack[_size - 2] = stack[_size - 1];
                --_size;
                correct_stack(params);
            }
            else {
                inplaceMerge(bottom + sum - first_len() - second_len(), bottom + sum - first_len(), bottom + sum, params);
                stack[_size - 2] += stack[_size - 1];
                --_size;
                correct_stack(params);
            }
        }
    }

public:
    RunsStack() : stack(new size_t[_INIT_STACK_SIZE]), _size(0), sum(0) {}

    size_t size() const {
        return _size;
    }

    size_t first_len() const {
        assert(_size > 0);
        return stack[_size - 1];
    }

    size_t second_len() const {
        assert(_size > 1);
        return stack[_size - 2];
    }

    size_t third_len() const {
        assert(_size > 2);
        return stack[_size - 3];
    }

    void finalMerge(const ITimSortParams& params) {
        while (_size >= 2) {
            inplaceMerge(bottom + sum - first_len() - second_len(),
                bottom + sum - first_len(), bottom + sum, params);
            stack[_size - 2] += stack[_size - 1];
            pop();
        }
    }

    template <class Compare>
    void finalMerge(Compare comp, const ITimSortParams& params) {
        while (_size >= 2) {
            inplaceMerge(bottom + sum - first_len() - second_len(),
                bottom + sum - first_len(), bottom + sum, comp, params);
            stack[_size - 2] += stack[_size - 1];
            pop();
        }
    }

    void push(const Run<RandomAccessIterator>& run, const ITimSortParams& params) {
        if (_size == 0)
            bottom = run.begin;

        stack[_size++] = run.len();
        sum += run.len();

        correct_stack(params);
    }

    template <class Compare>
    void push(const Run<RandomAccessIterator>& run, Compare comp, const ITimSortParams& params) {
        if (_size == 0)
            bottom = run.begin;

        stack[_size++] = run.len();
        sum += run.len();

        correct_stack(comp, params);
    }

    void pop() {
        assert(_size > 0);
        --_size;
    }
};