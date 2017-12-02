#include "stack.h"


template <int chunkSize>
class FixedAllocator {
private:
    mutable Stack allocated_, free_;
    const unsigned allocated_size_ = 1000000;
    mutable unsigned used_size_ = 1000000;

public:
    FixedAllocator() {}

    ~FixedAllocator() {
        while (!allocated_.empty()) {
            void* p = allocated_.top();
            ::operator delete(p);
            allocated_.pop();
        }
    }

    FixedAllocator(const FixedAllocator& another) {}

    void* allocateChunk() const {
        if (!free_.empty()) {
            void* ans = free_.top();
            free_.pop();
            return ans;
        }
        if (used_size_ == allocated_size_) {
            allocated_.push(::operator new(allocated_size_ * chunkSize));
            used_size_ = 0;
        }
        return (char*)allocated_.top() + (used_size_++) * chunkSize;
    }

    void deallocateChunk(void* p) const {
        free_.push(p);
    }
};