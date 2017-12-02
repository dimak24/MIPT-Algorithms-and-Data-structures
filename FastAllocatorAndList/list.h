#include "fastallocator.h"


namespace _list {
    template <typename T>
    struct Node {
        Node* prev_;
        Node* next_;
        T value_;

        Node(const T& value) : prev_(nullptr), next_(nullptr), value_(value) {}

        Node(T&& value) : prev_(nullptr), next_(nullptr), value_(std::move(value)) {}

        ~Node() {}
    };
}

template <typename T, class Allocator = FastAllocator<T>>
class List {
private:
    _list::Node<T> *head_, *tail_;
    int size_;
    Allocator alloc_;
    typedef typename Allocator::template rebind<_list::Node<T> >::other NodeAllocator;
    NodeAllocator node_alloc_;

    _list::Node<T>* insert_before_(_list::Node<T>* node, const T& value) {
        _list::Node<T>* newNode = node_alloc_.allocate(1);
        
        node_alloc_.construct(newNode, value);
        if (node->prev_) {
            newNode->prev_ = node->prev_;
            newNode->prev_->next_ = newNode;
        }
        newNode->next_ = node;
        node->prev_ = newNode;
        return newNode;
    }

    _list::Node<T>* insert_before_(_list::Node<T>* node, T&& value) {
        _list::Node<T>* newNode = node_alloc_.allocate(1);
        
        node_alloc_.construct(newNode, std::move(value));
        if (node->prev_) {
            newNode->prev_ = node->prev_;
            newNode->prev_->next_ = newNode;
        }
        newNode->next_ = node;
        node->prev_ = newNode;
        return newNode;
    }

    _list::Node<T>* insert_after_(_list::Node<T>* node, const T& value) {
        if (node->next_)
            return insert_before_(node->next_, value);
        node->next_ = node_alloc_.allocate(1);
        node_alloc_.construct(node->next_, value);
        node->next_->prev_ = node;
        return node->next_;
    }

    _list::Node<T>* insert_after_(_list::Node<T>* node, T&& value) {
        if (node->next_)
            return insert_before_(node->next_, value);
        node->next_ = node_alloc_.allocate(1);
        node_alloc_.construct(node->next_, std::move(value));
        node->next_->prev_ = node;
        return node->next_;
    }

    void erase_(_list::Node<T>* node) {
        if (node->prev_)
            node->prev_->next_ = node->next_;
        if (node->next_)
            node->next_->prev_ = node->prev_;
        node_alloc_.destroy(node);
        node_alloc_.deallocate(node, 1);
        node = nullptr;
    }

public:
    explicit List(const Allocator& alloc = Allocator())
        : head_(nullptr), tail_(nullptr), size_(0), alloc_(alloc) {}

    List(int count, const T& value = T(), const Allocator& alloc = Allocator())
        : size_(1), alloc_(alloc) {
            head_ = (_list::Node<T>*)node_alloc_.allocate(1);
            node_alloc_.construct(head_, value);
            tail_ = head_;
            while (size_ < count) {
                tail_->next_ = (_list::Node<T>*)node_alloc_.allocate(1);
                node_alloc_.construct(tail_->next_, value);
                tail_->next_->prev_ = tail_;
                tail_ = tail_->next_;
                ++size_;
            }
        }

    ~List() {
        _list::Node<T>* tmp;
        while(size_ > 0) {
            tmp = tail_->prev_;
            node_alloc_.destroy(tail_);
            node_alloc_.deallocate(tail_, 1);
            tail_ = tmp;
            --size_;
        }
    }

    int size() const {
        return size_;
    }

    void push_back(const T& value) {
        if (!tail_) {
            head_ = (_list::Node<T>*)node_alloc_.allocate(1);
            
            node_alloc_.construct(head_, value);
            tail_ = head_;
        }
        else {
            tail_ = insert_after_(tail_, value);
            if (size_ == 1)
                head_->next_ = tail_;
        }
        ++size_;
    }

    void push_back(T&& value) {
        if (!tail_) {
            head_ = (_list::Node<T>*)node_alloc_.allocate(1);
            
            node_alloc_.construct(head_, std::move(value));
            tail_ = head_;
        }
        else {
            tail_ = insert_after_(tail_, value);
            if (size_ == 1)
                head_->next_ = tail_;
        }
        ++size_;
    }

    List(const List& another) : size_(0), head_(nullptr), tail_(nullptr) {
        _list::Node<T>* tmp = another.head_;
        while (tmp) {
            push_back(tmp->value_);
            tmp = tmp->next_;
        }
    }

    List(List&& another) : size_(0), head_(nullptr), tail_(nullptr) {
        _list::Node<T>* tmp = another.head_;
        while (tmp) {
            push_back(std::move(tmp->value_));
            tmp = tmp->next_;
        }
    }

    List& operator=(const List& another) {
        if (head_) {
            node_alloc_.destroy(head_);
            node_alloc_.deallocate(head_, 1);
        }
        if (tail_) {
            node_alloc_.destroy(tail_);
            node_alloc_.deallocate(tail_, 1);
        }
        _list::Node<T>* tmp = another.head_;
        size_ = 0;
        head_ = nullptr;
        tail_ = nullptr;
        while (tmp) {
            push_back(tmp->value_);
            tmp = tmp->next_;
        }
        return *this;
    }

    List& operator=(List&& another) {
        if (head_) {
            node_alloc_.destroy(head_);
            node_alloc_.deallocate(head_, 1);
        }
        if (tail_) {
            node_alloc_.destroy(tail_);
            node_alloc_.deallocate(tail_, 1);
        }
        _list::Node<T>* tmp = another.head_;
        size_ = 0;
        head_ = nullptr;
        tail_ = nullptr;
        while (tmp) {
            push_back(std::move(tmp->value_));
            tmp = tmp->next_;
        }
        return *this;
    }

    void push_front(const T& value) {
        if (!head_) {
            head_ = (_list::Node<T>*)node_alloc_.allocate(1);
            
            node_alloc_.construct(head_, value);
            tail_ = head_;
        }
        else {
            head_ = insert_before_(head_, value);
            if (size_ == 1)
                tail_->prev_ = head_;
        }
        ++size_;
    }

    void push_front(T&& value) {
        if (!head_) {
            head_ = (_list::Node<T>*)node_alloc_.allocate(1);
            
            node_alloc_.construct(head_, std::move(value));
            tail_ = head_;
        }
        else {
            head_ = insert_before_(head_, value);
            if (size_ == 1)
                tail_->prev_ = head_;
        }
        ++size;
    }

    void pop_back() {
        if (!size_)
            return;
        if (!tail_->prev_) {
            node_alloc_.destroy(tail_);
            node_alloc_.deallocate(tail_, 1);
            tail_ = head_ = nullptr;
        }
        else {
            tail_ = tail_->prev_;
            erase_(tail_->next_);
            tail_->next_ = nullptr;
        }
        --size_;
    }

    void pop_front() {
        if (!size_)
            return;
        if (!head_->next_) {
            node_alloc_.destroy(head_);
            node_alloc_.deallocate(head_, 1);
            head_ = tail_ = nullptr;
        }
        else {
            head_ = head_->next_;
            erase_(head_->prev_);
            head_->prev_ = nullptr;
        }
        --size_;
    }

    _list::Node<T>* insert_before(_list::Node<T>* node, const T& value) {
        ++size_;
        return insert_before_(node, value);
    }

    _list::Node<T>* insert_before(_list::Node<T>* node, T&& value) {
        ++size_;
        return insert_before_(node, value);
    }

    _list::Node<T>* insert_after(_list::Node<T>* node, const T& value) {
        ++size_;
        return insert_after_(node, value);
    }

    _list::Node<T>* insert_after(_list::Node<T>* node, T&& value) {
        ++size_;
        return insert_after_(node, value);
    }

    void erase(_list::Node<T>* node) {
        --size_;
        erase_(node);
    }
};
