#pragma once

#include <iterator>
#include <cstring>

template <class T>
class Deque {
private:
    const long long _INIT_DEQUE_SIZE = 4;
    const long long _RESIZE_MULTIPLIER = 2;

    T* _buffer;
    T* _buffer_end;

    long long _capacity;
    long long _size;

    T* _begin;
    T* _end;

    enum EResizeTypes {
        ERT_Expansion,
        ERT_Reduction
    };

    void resize_if_necessary() {
        if (_size == _capacity)
            resize(ERT_Expansion);
        else if (_size * 4 == _capacity && _capacity > 4)
            resize(ERT_Reduction);
    }

    void resize(const EResizeTypes& how) {
        T* tmp = new T[_capacity];
        memcpy(tmp, _buffer, _capacity * sizeof(T));
        T* i = (_begin == _buffer_end ? tmp : (tmp + (_begin - _buffer) + 1));

        T* tmp_end = tmp + _capacity - 1;

        if (how == ERT_Expansion)
            _capacity *= _RESIZE_MULTIPLIER;
        else
            _capacity /= _RESIZE_MULTIPLIER;

        delete[] _buffer;
        _buffer = new T[_capacity];

        for (long long j = 0; j < _size; ++j) {
            _buffer[j] = *i;
            if (i == tmp_end)
                i = tmp;
            else
                ++i;
        }

        _buffer_end = _buffer + _capacity - 1;
        _begin = _buffer_end;
        _end = _buffer + _size;

        delete[] tmp;
    }

public:
    Deque() : _buffer(new T [_INIT_DEQUE_SIZE]), _buffer_end(_buffer + _INIT_DEQUE_SIZE - 1),
            _capacity(_INIT_DEQUE_SIZE), _size(0), _begin(_buffer_end), _end(_buffer) {}

    ~Deque() {
        delete[] _buffer;
    }

    Deque(const Deque& another) : _buffer(new T[another._capacity]), _capacity(another._capacity), _size(another._size) {
        T* i = (another._begin == another._buffer_end ? another._buffer : another._begin + 1);
        for (long long j = 0; j < _size; ++j) {
            _buffer[j] = *i;
            if (i == another._buffer_end)
                i = another._buffer;
            else
                ++i;
        }

        _buffer_end = _buffer + _capacity - 1;
        _begin = _buffer_end;
        _end = _buffer + _size;
    }

    const T& operator[](const long long& n) const {
        if ((_buffer_end - _begin)  >= n + 1)
            return *(_begin + n + 1);
        else
            return _buffer[n - (_buffer_end - _begin)];
    }

    T& operator[](const long long& n) {
        if ((_buffer_end - _begin) >= n + 1)
            return *(_begin + n + 1);
        else
            return _buffer[n - (_buffer_end - _begin)];
    }

    void push_back(const T& element) {
        resize_if_necessary();

        ++_size;
        *_end = T(element);
        if (_end == _buffer_end)
            _end = _buffer;
        else
            ++_end;
    }

    void pop_back() {
        resize_if_necessary();

        --_size;
        if (_end == _buffer)
            _end = _buffer_end;
        else
            --_end;
    }

    void push_front(const T& element) {
        resize_if_necessary();

        ++_size;
        *_begin = element;
        if (_begin == _buffer)
            _begin = _buffer_end;
        else
            --_begin;
    }

    void pop_front() {
        resize_if_necessary();

        --_size;
        if (_begin == _buffer_end)
            _begin = _buffer;
        else
            ++_begin;
    }

    long long size() const {
        return _size;
    }

    T& back() {
        return this->operator[](_size - 1);
    }

    const T& back() const {
        return this->operator[](_size - 1);
    }

    T& front() {
        return this->operator[](0);
    }

    const T& front() const {
        return this->operator[](0);
    }

    bool empty() const {
        return _size == 0;
    }

    struct DequeIteratorNode {
        T *_buffer, *_buffer_end;
        T *_begin, *_end;
        int _size;

        DequeIteratorNode(T* buffer, T* buffer_end, T* begin, T* end, int size) :
            _buffer(buffer), _buffer_end(buffer_end), _begin(begin), _end(end), _size(size) {}

        DequeIteratorNode() = default;
    };

    DequeIteratorNode getIteratorNode() const {
        return DequeIteratorNode(_buffer, _buffer_end, _begin, _end, _size);
    }

    template<class ValueType>
    class DequeIterator : public std::iterator<std::random_access_iterator_tag, T, std::ptrdiff_t, ValueType*, ValueType&> {
    private:
        ValueType* _pointer;

        DequeIteratorNode _node;

    public:
        DequeIterator(const DequeIterator& another) : _pointer(another._pointer), _node(another._node) {}

        DequeIterator(ValueType* pointer, DequeIteratorNode node) : _pointer(pointer), _node(node) {}

        DequeIterator() = default;

        ~DequeIterator() = default;

        DequeIterator& operator=(const DequeIterator& another) {
            _pointer = another._pointer;
            _node = another._node;
            return *this;
        }

        bool operator==(const DequeIterator& another) const {
            return _pointer == another._pointer;
        }

        bool operator!=(const DequeIterator& another) const {
            return _pointer != another._pointer;
        }

        bool operator<(const DequeIterator& another) const {
            if (_node._begin < _node._end)
                return _pointer < another._pointer;
            return (_pointer > _node._begin && another._pointer <= _node._end) ||
                   (_pointer > _node._begin && another._pointer > _node._begin && _pointer < another._pointer) ||
                   (_pointer <= _node._end && another._pointer <= _node._end && _pointer < another._pointer);
        }

        bool operator<=(const DequeIterator& another) const {
            return *this < another || *this == another;
        }

        bool operator>(const DequeIterator& another) const {
            return !(*this <= another);
        }

        bool operator >=(const DequeIterator& another) const {
            return !(*this < another);
        }

        DequeIterator& operator+=(const int& n) {
            if (n >= 0) {
                int dist = _node._buffer_end - _pointer;
                if (dist >= n) {
                    _pointer += n;
                    return *this;
                }
                _pointer = _node._buffer + (n - dist - 1);
                return *this;
            }
            int dist = _pointer - _node._buffer;
            if (dist >= -n) {
                _pointer += n;
                return *this;
            }
            _pointer = _node._buffer_end + n + dist + 1;
            return *this;
        }

        DequeIterator& operator-=(const int& n) {
            return *this += -n;
        }

        const DequeIterator operator+(const int& n) const {
            return DequeIterator(*this) += n;
        }

        const DequeIterator operator-(const int& n) const {
            return DequeIterator(*this) -= n;
        }

        int operator-(const DequeIterator& another) const {
            if (*this + _node._size == another)
                return _node._size;
            if (_node._begin < _node._end)
                return _pointer - another._pointer;
            if (_pointer > _node._begin && another._pointer <= _node._end)
                return -(_node._buffer_end - _pointer + 1 + (another._pointer - _node._buffer));
            if (another._pointer > _node._begin && _pointer <= _node._end)
                return (_node._buffer_end - another._pointer + 1 + (_pointer - _node._buffer));
            return _pointer - another._pointer;
        }

        DequeIterator& operator++() {
            return *this += 1;
        }

        DequeIterator& operator--() {
            return *this -= 1;
        }

        const DequeIterator& operator++(int) {
            DequeIterator tmp(*this);
            ++*this;
            return tmp;
        }

        const DequeIterator& operator--(int) {
            DequeIterator tmp(*this);
            --*this;
            return tmp;
        }

        ValueType& operator*() const {
            return *_pointer;
        }

        ValueType* operator->() const {
            return _pointer;
        }

        ValueType& operator[](const int& n) const {
            return *(*this + n);
        }

        operator DequeIterator<const T>() const {
            return DequeIterator<const T>(_pointer, _node);
        }
    };

    template<class ValueType>
    class ReverseDequeIterator : public std::iterator<std::random_access_iterator_tag, T, std::ptrdiff_t, ValueType*, ValueType&> { 
    private:
        ValueType* _pointer;

        DequeIteratorNode _node;

    public:
        ReverseDequeIterator(const ReverseDequeIterator& another) : _pointer(another._pointer), _node(another._node) {}

        ReverseDequeIterator(ValueType* pointer, DequeIteratorNode node) : _pointer(pointer), _node(node) {}

        ReverseDequeIterator() = default;

        ~ReverseDequeIterator() = default;

        ReverseDequeIterator& operator=(const ReverseDequeIterator& another) {
            _pointer = another._pointer;
            _node = another._node;
            return *this;
        }

        bool operator==(const ReverseDequeIterator& another) const {
            return _pointer == another._pointer;
        }

        bool operator!=(const ReverseDequeIterator& another) const {
            return _pointer != another._pointer;
        }

        bool operator<(const ReverseDequeIterator& another) const {
            if (_node._begin < _node._end)
                return _pointer > another._pointer;
            else
                return (another._pointer >= _node._begin && _pointer < _node._end) ||
                (_pointer >= _node._begin && another._pointer >= _node._begin && _pointer > another._pointer) ||
                (_pointer < _node._end && another._pointer < _node._end && _pointer > another._pointer);
        }

        bool operator<=(const ReverseDequeIterator& another) const {
            return *this < another || *this == another;
        }

        bool operator>(const ReverseDequeIterator& another) const {
            return !(*this <= another);
        }

        bool operator>=(const ReverseDequeIterator& another) const {
            return !(*this < another);
        }

        ReverseDequeIterator& operator+=(const int& n) {
            if (n >= 0) {
                int dist = _pointer - _node._buffer;
                if (dist >= n) {
                    _pointer -= n;
                    return *this;
                }
                _pointer = _node._buffer_end - (n - dist - 1);
                return *this;
            }
            int dist = _node._buffer_end - _pointer;
            if (dist >= -n) {
                _pointer -= n;
                return *this;
            }
            _pointer = _node._buffer - (n + dist + 1);
            return *this;
        }

        ReverseDequeIterator& operator-=(const int& n) {
            return *this += -n;
        }

        const ReverseDequeIterator operator+(const int& n) const {
            return ReverseDequeIterator(*this) += n;
        }

        const ReverseDequeIterator operator-(const int& n) const {
            return ReverseDequeIterator(*this) -= n;
        }

        ReverseDequeIterator& operator--() {
            return *this -= 1;
        }

        ReverseDequeIterator& operator++() {
            return *this += 1;
        }

        const ReverseDequeIterator& operator--(int) {
            ReverseDequeIterator tmp(*this);
            --*this;
            return tmp;
        }

        const ReverseDequeIterator& operator++(int) {
            ReverseDequeIterator tmp(*this);
            ++*this;
            return tmp;
        }

        int operator-(const ReverseDequeIterator& another) const {
            if (*this + _node._size == another)
                return _node._size;
            if (_node._begin < _node._end)
                return another._pointer - _pointer;
            if (_pointer >= _node._begin && another._pointer < _node._end)
                return (_node._buffer_end - _pointer + 1 + (another._pointer - _node._buffer));
            if (another._pointer >= _node._begin && _pointer < _node._end)
                return -(_node._buffer_end - another._pointer + 1 + (_pointer - _node._buffer));
            return another._pointer - _pointer;
        }

        ValueType& operator*() const {
            return *_pointer;
        }

        ValueType* operator->() const {
            return _pointer;
        }

        ValueType& operator[](const int& n) const {
            return *(*this + n);
        }

        operator ReverseDequeIterator<const T>() const {
            return ReverseDequeIterator<const T>(_pointer, DequeIteratorNode(_node));
        }
    };

    typedef DequeIterator<T> iterator;

    typedef DequeIterator<const T> const_iterator;

    typedef ReverseDequeIterator<T> reverse_iterator;

    typedef ReverseDequeIterator<const T> const_reverse_iterator;

    iterator begin() {
        return iterator(&(this->operator[](0)), getIteratorNode());
    }

    iterator end() {
        return iterator(_end, getIteratorNode());
    }

    const_iterator begin() const {
        return const_iterator(&(this->operator[](0)), getIteratorNode());
    }

    const_iterator end() const {
        return const_iterator(_end, getIteratorNode());
    }

    const_iterator cbegin() const {
        return const_iterator(&(this->operator[](0)), getIteratorNode());
    }

    const_iterator cend() const {
        return const_iterator(_end, getIteratorNode());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(&(this->operator[](_size - 1)), getIteratorNode());
    }

    reverse_iterator rend() {
        return reverse_iterator(_begin, getIteratorNode());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(&(this->operator[](_size - 1)), getIteratorNode());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(_begin, getIteratorNode());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(&(this->operator[](_size - 1)), getIteratorNode());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(_begin, getIteratorNode());
    }
};