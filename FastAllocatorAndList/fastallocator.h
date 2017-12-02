#include "fixedallocator.h"
#include <iostream>


template <typename T>
class FastAllocator {
private:
    static FixedAllocator<24> fa24;

public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef int size_type;
    typedef std::ptrdiff_t difference_type;
    typedef std::true_type propagate_on_container_move_assignment;

    template <class U> 
    struct rebind { typedef FastAllocator<U> other; };

    FastAllocator() {}

    ~FastAllocator() {}

    FastAllocator(const FastAllocator&) {}

    FastAllocator(FastAllocator&&) {}

    pointer allocate(const int& n) const {
        int size = sizeof(T) * n;
        if (size == 24)
            return reinterpret_cast<pointer>(fa24.allocateChunk());
        return reinterpret_cast<pointer>(::operator new(size));
    }

    void deallocate(pointer p, const int& n) const {
        int size = sizeof(T) * n;
        if (size == 24)
            fa24.deallocateChunk(p);
        else
            ::operator delete(p);
    }

    template<class U, class... Args>
    void construct(U* p, Args&&... args) const {
        ::new((void*)p) U(std::forward<Args>(args)...);
    }

    template<class U>
    void destroy(U* p ) const {
        p->~U();
    }
};

template<class T1, class T2>
bool operator==(const FastAllocator<T1>& lhs, const FastAllocator<T2>& rhs) {
    return true;
}

template<class T1, class T2>
bool operator!=(const FastAllocator<T1>& lhs, const FastAllocator<T2>& rhs) {
    return false;
}

template <typename T>
FixedAllocator<24> FastAllocator<T>::fa24;
