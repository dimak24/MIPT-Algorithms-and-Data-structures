#include <iostream>
#include <memory>

template <typename T>
class UniquePtr {
private:
    T* ptr_;

public:
    explicit UniquePtr(T* ptr) : ptr_(ptr) {}

    UniquePtr(UniquePtr&& another) : ptr_(another.release()) {}

    UniquePtr(const UniquePtr& another) = delete;

    ~UniquePtr() {
        reset();
    }

    T* get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    T* release() {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* ptr) {
        if (ptr_)
            delete ptr_;
        ptr_ = ptr;
    }

    void reset() {
        reset(nullptr);
    }

    UniquePtr& operator=(UniquePtr&& another) {
        reset(another.release());
        return *this;
    }

    void swap(UniquePtr& another) {
        std::swap(*this, another);
    }
};

template <typename T> class WeakPtr;

template <typename T>
class SharedPtr {
friend class WeakPtr<T>;
private:
    T* ptr_;
    std::size_t* count_;
    std::size_t* soft_links_count_;

public:
    explicit SharedPtr(T* ptr) : ptr_(ptr), count_(new std::size_t(1)), soft_links_count_(new std::size_t(0)) {}

    SharedPtr() : ptr_(nullptr), count_(nullptr), soft_links_count_(nullptr) {}

    SharedPtr(const SharedPtr& another)
        : ptr_(another.ptr_), count_(another.count_), soft_links_count_(another.soft_links_count_) {
            if (count_)
                ++*count_;
        }

    SharedPtr(SharedPtr&& another)
        : ptr_(another.ptr_), count_(another.count_), soft_links_count_(another.soft_links_count_) {
            if (count_)
                ++*count_;
        }

    explicit SharedPtr(const WeakPtr<T>& weak)
        : ptr_(weak.ptr_), count_(weak.count_), soft_links_count_(weak.soft_links_count_) {
            if (count_)
                ++*count_;
        }

    void swap(SharedPtr& another) {
        std::swap(ptr_, another.ptr_);
        std::swap(count_, another.count_);
        std::swap(soft_links_count_, another.soft_links_count_);
    }

    SharedPtr& operator=(const SharedPtr& another) {
        SharedPtr(another).swap(*this);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& another) {
        SharedPtr(std::move(another)).swap(*this);
        return *this;
    }

    ~SharedPtr() {
        if (count_) {
            --*count_;
            if (!*count_) {
                delete ptr_;
                if (!soft_links_count_ || !*soft_links_count_) {
                    delete count_;
                    delete soft_links_count_;
                }
            }
        }
        else
            delete ptr_;
    }

    std::size_t use_count() const {
        return count_ ? *count_ : 0;
    }

    bool expired() const {
        return !count_ || *count_ == 0;
    }

    T* get() const {
        return ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    void reset() {
        SharedPtr<T>().swap(*this);
    }

    template<class U> 
    void reset(U* ptr) {
        SharedPtr<T>(ptr).swap(*this);
    }
};


template <typename T>
class WeakPtr {
friend class SharedPtr<T>;
private:
    T* ptr_;
    std::size_t* count_;
    std::size_t* soft_links_count_;

public:
    WeakPtr(const SharedPtr<T>& shared)
        : ptr_(shared.ptr_), count_(shared.count_), soft_links_count_(new std::size_t(1)) {}

    WeakPtr(const WeakPtr& another)
        : ptr_(another.ptr_), count_(another.count_), soft_links_count_(another.soft_links_count_) {
            if (soft_links_count_)
                ++*soft_links_count_;
        }

    WeakPtr(WeakPtr&& another)
        : ptr_(another.ptr_), count_(another.count_), soft_links_count_(another.soft_links_count_) {
            if (soft_links_count_)
                ++*soft_links_count_;
        }

    WeakPtr() : ptr_(nullptr), count_(nullptr), soft_links_count_(nullptr) {}

    void swap(WeakPtr& another) {
        std::swap(ptr_, another.ptr_);
        std::swap(count_, another.count_);
        std::swap(soft_links_count_, another.soft_links_count_);
    }

    WeakPtr& operator=(const WeakPtr& another) {
        WeakPtr<T>(another).swap(*this);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& another) {
        WeakPtr<T>(std::move(another)).swap(*this);
        return *this;
    }

    WeakPtr& operator=(const SharedPtr<T>& another) {
        count_ = another.count_;
        ptr_ = another.ptr_;
        if (soft_links_count_)
            --*soft_links_count_;
        soft_links_count_ = another.soft_links_count_;
        if (soft_links_count_)
            ++*soft_links_count_;
        return *this;
    }

    ~WeakPtr() {
        if (soft_links_count_) {
            --*soft_links_count_;
            if (!*soft_links_count_) {
                delete soft_links_count_;
                if (count_ && !*count_)
                    delete count_;
            }
        }
        else if (count_ && !*count_)
            delete count_;
    }

    std::size_t use_count() const {
        return count_ ? *count_ : 0;
    }

    bool expired() const {
        return !count_ || *count_ == 0;
    }

    SharedPtr<T> lock() const {
        return expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
    }

    void reset() {
        WeakPtr<T>().swap(*this);
    }
};