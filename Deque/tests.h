#pragma once

#include "deque.h"
#include <ctime>
#include <cstdlib>
#include "gtest/gtest.h"

class DequeTest : public ::testing::Test {
protected:
    Deque<int> q0, q1, q2, q3;

    virtual void SetUp() {
        for (int i = 0; i < 1000; ++i) {
            q1.push_back(i);
            q2.push_front(i);
            q3.push_back(i);
        }
    }
};

TEST_F(DequeTest, SizeEmptyPushPopBackFront) {
    ASSERT_TRUE(q0.empty());
    ASSERT_TRUE(!q1.empty());
    EXPECT_EQ(q1.size(), 1000);
    for (int i = 0; i < 1000; ++i) {

        EXPECT_EQ(q1.front(), i);
        q1.pop_front();

        EXPECT_EQ(q2.back(), i);
        q2.pop_back();

        int n = rand() % 1000;
        EXPECT_EQ(q3[n], n);
    }
}

TEST_F(DequeTest, IteratorTraits) {
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::iterator >::iterator_category) == typeid(std::random_access_iterator_tag));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_iterator >::iterator_category) == typeid(std::random_access_iterator_tag));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::reverse_iterator >::iterator_category) == typeid(std::random_access_iterator_tag));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_reverse_iterator >::iterator_category) == typeid(std::random_access_iterator_tag));

    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::iterator >::value_type) == typeid(int));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_iterator >::value_type) == typeid(int));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::reverse_iterator >::value_type) == typeid(int));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_reverse_iterator >::value_type) == typeid(int));

    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::iterator >::difference_type) == typeid(std::ptrdiff_t));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_iterator >::difference_type) == typeid(std::ptrdiff_t));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::reverse_iterator >::difference_type) == typeid(std::ptrdiff_t));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_reverse_iterator >::difference_type) == typeid(std::ptrdiff_t));

    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::iterator >::pointer) == typeid(int*));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_iterator >::pointer) == typeid(const int*));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::reverse_iterator >::pointer) == typeid(int*));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_reverse_iterator >::pointer) == typeid(const int*));

    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::iterator >::reference) == typeid(int&));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_iterator >::reference) == typeid(const int&));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::reverse_iterator >::reference) == typeid(int&));
    ASSERT_TRUE(typeid(std::iterator_traits<Deque<int>::const_reverse_iterator >::reference) == typeid(const int&));
}

TEST_F(DequeTest, Iterators) {
    Deque<int>::iterator itBegin = q3.begin(), itEnd = q3.end();
    Deque<int>::reverse_iterator ritBegin = q3.rbegin(), ritEnd = q3.rend();

    int n = 0;

    for (Deque<int>::iterator it = itBegin; it != itEnd; ++it, ++n) {
        EXPECT_EQ(*it, q3[n]);
        EXPECT_EQ(it, itBegin + n);
        EXPECT_EQ(*it, itBegin[n]);
    }

    n = 0;

    for (Deque<int>::reverse_iterator it = ritBegin; it != ritEnd; ++it, ++n) {
        EXPECT_EQ(*it, q3[999 - n]);
        EXPECT_EQ(it, ritBegin + n);
        EXPECT_EQ(*it, ritBegin[n]);
    }
}

TEST_F(DequeTest, Complexity) {
    double a = 0.1;                          //time < a * n (a = 0.1) 
    srand(time(0));
    size_t operations_number[] = { 100000, 1000000, 10000000, 20000000, 30000000,
        rand() % 10000 * 1000u + rand(), rand() % 10000 * 1000u + rand(),
        rand() % 10000 * 1000u + rand(), rand() % 10000 * 1000u + rand(), rand() % 10000 * 1000u + rand() };
    size_t k = 10;
    for (size_t i = 0; i < k; ++i) {
        Deque<int> d;
        size_t n = operations_number[i];
        time_t t = clock();
        for (size_t j = 0; j < n; ++j)
            d.push_back(1000);
        t = clock() - t;
        double f = a * double(n);
        ASSERT_TRUE(t < f);
    }
}