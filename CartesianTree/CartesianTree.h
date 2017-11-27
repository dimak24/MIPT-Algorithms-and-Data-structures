#include <iostream>
#include <functional>


class CartesianTree {
private:
    struct CTVertex {
        CTVertex *left, *right, *parent;
        std::size_t priority;
        long long value;
        long long leftistValue, rightistValue;

        std::size_t size;
        long long sum;
        long long add;
        bool isAssigned;
        long long assignedValue;
        bool isReversed;
        std::size_t rightDecreasingSequenceSize;
        std::size_t leftIncreasingSequenceSize;
        std::size_t rightIncreasingSequenceSize;
        std::size_t leftDecreasingSequenceSize;

        explicit CTVertex(const int& value) : priority(rand()), value(value), 
                                     sum(value), size(1), add(0), isAssigned(false),
                                     isReversed(false), rightDecreasingSequenceSize(1),
                                     leftIncreasingSequenceSize(1),
                                     rightIncreasingSequenceSize(1),
                                     leftDecreasingSequenceSize(1),
                                     leftistValue(value), rightistValue(value) {}

        ~CTVertex() {
            if (left)
                delete left;
            if (right)
                delete right;
        }

        CTVertex(const CTVertex& another) 
            : left(another.left ? new CTVertex(*another.left) : nullptr),
              right(another.right ? new CTVertex(*another.right) : nullptr),
              parent(another.parent ? new CTVertex(*another.parent) : nullptr),
              priority(another.priority),
              value(another.value),
              leftistValue(another.leftistValue),
              rightistValue(another.rightistValue),
              size(another.size),
              sum(another.sum),
              add(another.add),
              isAssigned(another.isAssigned),
              assignedValue(another.assignedValue),
              isReversed(another.isReversed),
              rightDecreasingSequenceSize(another.rightDecreasingSequenceSize),
              leftIncreasingSequenceSize(another.leftIncreasingSequenceSize),
              rightIncreasingSequenceSize(another.rightIncreasingSequenceSize),
              leftDecreasingSequenceSize(another.leftDecreasingSequenceSize) {}
    };

    static std::size_t size_(CTVertex* tree) {
        return (tree ? tree->size : 0);
    }

    static long long sum_(CTVertex* tree) {
        return (tree ? (tree->isAssigned ? tree->assignedValue * tree->size : 
            tree->sum + tree->add * tree->size) : 0);
    }

    static long long value_(CTVertex* tree) {
        return (tree ? 
            (tree->isAssigned ? tree->assignedValue : tree->value + tree->add) : 0);
    }

    CTVertex* left_(CTVertex* vertex) {
        return (vertex ? (vertex->isReversed ? vertex->right : vertex->left) : 0);
    }

    CTVertex* right_(CTVertex* vertex) {
        return (vertex ? (vertex->isReversed ? vertex->left : vertex->right) : 0);
    }

    static std::size_t rightIncreasingSequenceSize_(CTVertex* tree) {
        return (tree ? (tree->isAssigned ? tree->size :
            (tree->isReversed ? tree->leftDecreasingSequenceSize :
            tree->rightIncreasingSequenceSize)) : 0);
    }

    static std::size_t leftIncreasingSequenceSize_(CTVertex* tree) {
        return (tree ? (tree->isAssigned ? tree->size :
            (tree->isReversed ? tree->rightDecreasingSequenceSize :
            tree->leftIncreasingSequenceSize)) : 0);
    }

    static std::size_t rightDecreasingSequenceSize_(CTVertex* tree) {
        return (tree ? (tree->isAssigned ? tree->size :
            (tree->isReversed ? tree->leftIncreasingSequenceSize :
            tree->rightDecreasingSequenceSize)) : 0);
    }

    static std::size_t leftDecreasingSequenceSize_(CTVertex* tree) {
        return (tree ? (tree->isAssigned ? tree->size :
            (tree->isReversed ? tree->rightIncreasingSequenceSize :
            tree->leftDecreasingSequenceSize)) : 0);
    }

    static long long getLeftist_(CTVertex* tree) {
        return (tree->isAssigned ? tree->assignedValue :
            (tree->isReversed ? tree->rightistValue : tree->leftistValue) + tree->add);
    }

    static long long getRightist_(CTVertex* tree) {
        return (tree->isAssigned ? tree->assignedValue :
            (tree->isReversed ? tree->leftistValue : tree->rightistValue) + tree->add);
    }

    static void push_if_assigned_(CTVertex* vertex) {
        if (vertex->isAssigned) {
            if (vertex->left) {
                vertex->left->add = 0;
                vertex->left->isAssigned = true;
                vertex->left->assignedValue = vertex->assignedValue;
            }
            if (vertex->right) {
                vertex->right->add = 0;
                vertex->right->isAssigned = true;
                vertex->right->assignedValue = vertex->assignedValue;
            }
            vertex->value = vertex->assignedValue;
            vertex->leftistValue = vertex->assignedValue;
            vertex->rightistValue = vertex->assignedValue;
            vertex->sum = vertex->assignedValue * vertex->size;
            vertex->isAssigned = false;
            vertex->rightDecreasingSequenceSize = vertex->size;
            vertex->leftIncreasingSequenceSize = vertex->size;
            vertex->rightIncreasingSequenceSize = vertex->size;
            vertex->leftDecreasingSequenceSize = vertex->size;
        }
    }

    static void push_if_added_(CTVertex* vertex) {
        if (vertex->add) {
            if (vertex->left) {
                if (vertex->left->isAssigned)
                    vertex->left->assignedValue += vertex->add;
                else
                    vertex->left->add += vertex->add;
            }
            if (vertex->right) {
                if (vertex->right->isAssigned)
                    vertex->right->assignedValue += vertex->add;
                else
                    vertex->right->add += vertex->add;
            }
            vertex->value += vertex->add;
            vertex->leftistValue += vertex->add;
            vertex->rightistValue += vertex->add;
            vertex->sum += vertex->size * vertex->add;
            vertex->add = 0;
        }
    }

    static void push_if_reversed_(CTVertex* vertex) {
        if (vertex->isReversed) {
            if (vertex->left)
                vertex->left->isReversed ^= true;
            if (vertex->right)
                vertex->right->isReversed ^= true;
            std::swap(vertex->left, vertex->right);
            vertex->isReversed = false;
            std::swap(vertex->leftIncreasingSequenceSize,
                      vertex->rightDecreasingSequenceSize);
            std::swap(vertex->leftDecreasingSequenceSize,
                      vertex->rightIncreasingSequenceSize);
            std::swap(vertex->leftistValue, vertex->rightistValue);
        }
    }

    static void push_(CTVertex* vertex) {
        if (vertex) {
            push_if_assigned_(vertex);
            push_if_added_(vertex);
            push_if_reversed_(vertex);    
        }
    }

    template <class Comparator>
    static void updateSequenceSize_(CTVertex* vertex, std::size_t (*getSequenceSize)(CTVertex*), 
            long long (*first)(CTVertex*), long long (*second)(CTVertex*),
            CTVertex* firstChild, CTVertex* secondChild, std::size_t& result, Comparator cmp) {
        result = getSequenceSize(firstChild);
        if (result == size_(firstChild) && (!firstChild || !cmp(value_(vertex), first(firstChild))))
            ++result;
        if ((result == size_(firstChild) + 1) && secondChild && !cmp(second(secondChild), value_(vertex)))
            result += getSequenceSize(secondChild);
    }


    static void update_(CTVertex* vertex) {
        if (!vertex)
            return;
        vertex->size = size_(vertex->left) + size_(vertex->right) + 1;
        vertex->sum = sum_(vertex->left) + sum_(vertex->right) + value_(vertex);
        vertex->leftistValue = vertex->left ? getLeftist_(vertex->left) : value_(vertex);
        vertex->rightistValue = vertex->right ? getRightist_(vertex->right) : value_(vertex);
        
        updateSequenceSize_(vertex, rightDecreasingSequenceSize_, getLeftist_, getRightist_,
                            vertex->right, vertex->left, vertex->rightDecreasingSequenceSize, std::less<long long>());
        updateSequenceSize_(vertex, leftIncreasingSequenceSize_, getRightist_, getLeftist_,
                            vertex->left, vertex->right, vertex->leftIncreasingSequenceSize, std::less<long long>());
        updateSequenceSize_(vertex, leftDecreasingSequenceSize_, getRightist_, getLeftist_,
                            vertex->left, vertex->right, vertex->leftDecreasingSequenceSize, std::greater<long long>());
        updateSequenceSize_(vertex, rightIncreasingSequenceSize_, getLeftist_, getRightist_,
                            vertex->right, vertex->left, vertex->rightIncreasingSequenceSize, std::greater<long long>());
    }

    static void merge_(CTVertex* leftTree, CTVertex* rightTree, CTVertex*& result) {
        push_(leftTree);
        push_(rightTree);
        if (!leftTree || !rightTree) {
            result = (leftTree ? leftTree : rightTree);
            update_(result);
            return;
        }
        if (leftTree->priority >= rightTree->priority) {
            result = leftTree;
            merge_(result->right, rightTree, result->right);
            if (result->right)
                result->right->parent = result;
        }
        else {
            result = rightTree;
            merge_(leftTree, result->left, result->left);
            if (result->left)
                result->left->parent = result;
        }
        update_(result);
    }

    static void split_(CTVertex* tree, const std::size_t& leftSize,
               CTVertex*& leftTree, CTVertex*& rightTree) {
        if (!tree) {
            leftTree = nullptr;
            rightTree = nullptr;
            return;
        }
        push_(tree);
        if (size_(tree->left) >= leftSize) {
            split_(tree->left, leftSize, leftTree, tree->left);
            rightTree = tree;
            if (leftTree)
                leftTree->parent = nullptr;
            if (rightTree->left)
                rightTree->left->parent = rightTree;
        }
        else {
            split_(tree->right, leftSize - size_(tree->left) - 1,
                  tree->right, rightTree);
            leftTree = tree;
            if (rightTree)
                rightTree->parent = nullptr;
            if (leftTree->right)
                leftTree->right->parent = leftTree;
        }
        update_(tree);
    }

    template <class Comparator>
    static std::size_t extremeVertexValue_(CTVertex* tree, const long long& val, Comparator cmp) {
        push_(tree);
        if (tree->left && !cmp(value_(tree), val))
            return extremeVertexValue_(tree->left, val, cmp);
        if (tree->right && cmp(value_(tree), val)) {
            auto t = extremeVertexValue_(tree->right, val, cmp);
            if (t)
                return size_(tree->left) + 1 + t;
        }
        if (cmp(value_(tree), val))
            return size_(tree->left) + 1;
        return 0;
    }

    void insert_(CTVertex*& tree, const std::size_t& index, const long long& value) {
        if (!tree) {
            tree = new CTVertex(value);
            return;
        }
        CTVertex *leftTree, *rightTree;
        split_(tree, index, leftTree, rightTree);
        CTVertex* newElement = new CTVertex(value);
        merge_(leftTree, newElement, tree);
        merge_(tree, rightTree, tree);
    }

    void erase_(CTVertex*& tree, const std::size_t& index) {
        CTVertex *leftTree, *rightTree, *erasedElement;
        split_(tree, index, leftTree, rightTree);
        split_(rightTree, 1, erasedElement, rightTree);
        merge_(leftTree, rightTree, tree);
    }

    template <class OperationOnSegment>
    void doSomethingOnSegment(CTVertex* tree, const std::size_t& l, const std::size_t& r,
                              OperationOnSegment something) {
        CTVertex *leftTree, *middleTree, *rightTree;
        split_(tree, l, leftTree, middleTree);
        split_(middleTree, r - l + 1, middleTree, rightTree);
        something(middleTree);
        merge_(leftTree, middleTree, tree);
        merge_(tree, rightTree, tree);
    }

    template <class Comparator>
    static void followingPermutation_(CTVertex* middleTree,
            std::size_t (*rightSequenceSize)(CTVertex*), Comparator cmp) {
        if (size_(middleTree) == rightSequenceSize(middleTree))
            middleTree->isReversed ^= true;
        else {
            CTVertex *subMiddleTree1, *subMiddleTree2, *subLeftTree, *pred;
            split_(middleTree, size_(middleTree) - rightSequenceSize(middleTree),
                   subMiddleTree1, subMiddleTree2);
            split_(subMiddleTree1, subMiddleTree1->size - 1, subLeftTree, pred);
            std::size_t upperBound = extremeVertexValue_(subMiddleTree2, value_(pred), cmp);
            CTVertex *subSubLeft, *subSubRight, *subSubMiddle;
            split_(subMiddleTree2, upperBound - 1, subSubLeft, subSubMiddle);
            split_(subSubMiddle, 1, subSubMiddle, subSubRight);
            std::swap(subSubMiddle->value, pred->value);
            std::swap(subSubMiddle->sum, pred->sum);
            merge_(subSubLeft, subSubMiddle, subMiddleTree2);
            merge_(subMiddleTree2, subSubRight, subMiddleTree2);
            subMiddleTree2->isReversed ^= true;
            merge_(subLeftTree, pred, subMiddleTree1);
            merge_(subMiddleTree1, subMiddleTree2, middleTree);
        }
    }

    long long getSum_(CTVertex* tree, const std::size_t& l, const std::size_t& r) {
        long long ans;
        doSomethingOnSegment(tree, l, r, [&ans](CTVertex* tree) {
                                            ans = sum_(tree);
                                        });
        return ans;
    }

    void addOnSegment_(CTVertex* tree, const std::size_t& l, const std::size_t& r,
                       const long long& add) {
        doSomethingOnSegment(tree, l, r, [&add](CTVertex* tree) {
                                             tree->add += add;
                                         });
    }

    void assignOnSegment_(CTVertex* tree, const std::size_t& l, const std::size_t& r,
                         const long long& assignedValue) {
        doSomethingOnSegment(tree, l, r, [&assignedValue](CTVertex* tree) {
                                             tree->isAssigned = true;
                                             tree->assignedValue = assignedValue;
                                         });
    }

    void reverse_(CTVertex* tree, const std::size_t& l, const std::size_t& r) {
        doSomethingOnSegment(tree, l, r, [](CTVertex* tree) {
                                             tree->isReversed ^= true;
                                         });
    }

    void nextPermutation_(CTVertex* tree, const std::size_t& l, const std::size_t& r) {
        doSomethingOnSegment(tree, l, r, [](CTVertex* tree){
                    followingPermutation_(tree, rightDecreasingSequenceSize_, std::greater<long long>());
                    });
    }

    void prevPermutation_(CTVertex* tree, const std::size_t& l, const std::size_t& r) {
        doSomethingOnSegment(tree, l, r, [](CTVertex* tree){
                    followingPermutation_(tree, rightIncreasingSequenceSize_, std::less<long long>());
                    });
    }

    void inOrder_(CTVertex* tree) {
        if (!tree)
            return;
        push_(tree);
        inOrder_(tree->left);
        std::cout << value_(tree) << " ";
        inOrder_(tree->right);
    }

    void read_(CTVertex*& tree) {
        long long n, v, prior;
        std::cin >> n;
        for (int i = 0; i < n; ++i) {
            std::cin >> v;
            CTVertex* newVertex = new CTVertex(v);
            if (!tree)
                tree = newVertex;
            else if (newVertex->priority >= tree->priority) {
                tree->parent = newVertex;
                newVertex->left = tree;
                std::swap(tree, newVertex);
                update_(tree);
            }
            else {
                CTVertex* tmp = tree;
                while (tmp->right && tmp->right->priority > newVertex->priority)
                    tmp = tmp->right;
                if (!tmp->right) {
                    tmp->right = newVertex;
                    newVertex->parent = tmp;
                }
                else {
                    newVertex->left = tmp->right;
                    newVertex->left->parent = newVertex;
                    tmp->right = newVertex;
                    newVertex->parent = tmp;
                    tmp = newVertex;
                }
                while (tmp) {
                    update_(tmp);
                    tmp = tmp->parent;
                }
            }
        }
    }


    CTVertex* tree_;

public:
    CartesianTree() : tree_(nullptr) {}

    CartesianTree(const CartesianTree& another) : 
                        tree_(new CTVertex(*another.tree_)) {}

    ~CartesianTree() {}

    void insert(const std::size_t& index, const long long& value) {
        insert_(tree_, index, value);
    }

    void erase(const std::size_t& index) {
        erase_(tree_, index);
    }

    long long getSum(const std::size_t& l, const std::size_t& r) {
        return getSum_(tree_, l, r);
    }

    void addOnSegment(const std::size_t& l, const std::size_t& r,
                      const long long& add) {
        addOnSegment_(tree_, l, r, add);
    }

    void assignOnSegment(const std::size_t& l, const std::size_t& r,
                         const long long& assignedValue) {
        assignOnSegment_(tree_, l, r, assignedValue);
    }

    void reverse(const std::size_t& l, const std::size_t& r) {
        reverse_(tree_, l, r);
    }

    void nextPermutation(const std::size_t& l, const std::size_t& r) {
        nextPermutation_(tree_, l, r);
    }

    void prevPermutation(const std::size_t& l, const std::size_t& r) {
        prevPermutation_(tree_, l, r);
    }

    void inOrder() {
        inOrder_(tree_);
    }

    void read() {
        read_(tree_);
    }
};
