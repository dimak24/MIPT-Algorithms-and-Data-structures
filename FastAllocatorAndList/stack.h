#include <cstring>


struct Stack {
    void** buf;
    int size;
    int capacity;
    ~Stack() {
        ::delete[](buf);
    }
    Stack() : size(0), capacity(100000000) {
        buf = new void* [capacity];
    }
    void push(void* value) {
        buf[size++] = value;
        if (size == capacity) {
            void** newBuf = new void*[capacity];
            memcpy(newBuf, buf, size * sizeof(void*));
            ::operator delete[](buf);
            buf = new void*[capacity *= 2];
            memcpy(buf, newBuf, size * sizeof(void*));
            ::operator delete[](newBuf);
        }
    }
    void pop() {
        --size;
    }
    void* top() {
        return buf[size - 1];
    }

    bool empty() {
        return size == 0;
    }
};