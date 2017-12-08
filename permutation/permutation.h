#include <iostream>
#include <cassert>

class Permutation {

private:
    int* perm;
    int n = 0;

    void swap(const int& x, const int& y) {
        perm[x] += perm[y];
        perm[y] = perm[x] - perm[y];
        perm[x] -= perm[y];
    }

    size_t derangements(int *start, int *finish, int* buffer) const {
        if (start + 1 != finish) {
            int* middle = start + (finish - start) / 2;
            size_t ans = derangements(start, middle, buffer) + derangements(middle, finish, buffer);
            int* i = start;
            int* j = middle;
            int p = 0;
            int* new_buffer = new int[finish - start];
            size_t inv = 0;
            while (i < middle && j < finish) {
                if (*j <= *i) {
                    new_buffer[p++] = *(j++);
                    ++inv;
                }
                else {
                    new_buffer[p++] = *(i++);
                    if (i < middle)
                        inv += (j - middle);
                }
            }
            if (i < middle)
                inv += (middle - i - 1)*(j - middle);
            while (i < middle)
                new_buffer[p++] = *(i++);
            while (j < finish)
                new_buffer[p++] = *(j++);
            for (int i = 0; i < finish - start; ++i)
                *(start + i) = new_buffer[i];
            delete[] new_buffer;
            return inv + ans;
        }
        return 0;
    }

public:
    Permutation() = delete;

    ~Permutation() {
        if (n)
            delete[] perm;
    }

    Permutation(const Permutation& p) : perm(new int [p.n]), n(p.n) {
        for (int i = 0; i < n; ++i)
            perm[i] = p.perm[i];
    }

    explicit Permutation(const unsigned int& k) : perm(new int [k]), n(k) {
        for (int i = 0; i < n; ++i)
            perm[i] = i;
    }

    explicit Permutation(const unsigned int& k, const int* p) : perm(new int [k]), n(k) {
        for (int i = 0; i < n; ++i)
            perm[i] = p[i];
    }

    Permutation& operator=(const Permutation& p) {
        delete[] perm;
        n = p.n;
        perm = new int[n];
        for (int i = 0; i < n; ++i)
            perm[i] = p.perm[i];
        return *this;
    }

    const int& operator[](const int& k) const {
        assert(k < n);
        return perm[k];
    }
    
    Permutation& operator*=(const Permutation& p) {
        assert(n == p.n);
        int* perm1 = new int[n];
        for (int i = 0; i < n; ++i)
            perm1[i] = perm[p[i]];
        for (int i = 0; i < n; ++i)
            perm[i] = perm1[i];
        delete[] perm1;
        return *this;
    }

    const Permutation operator*(const Permutation& p) const {
        assert(n == p.n);
        Permutation res(n, perm);
        return (res *= p);
    }

    const Permutation next() const {
        Permutation p(n, perm);
        ++p;
        return p;
    }

    Permutation& operator++() {
        for (int i = n - 1; i >= 0; --i) {
            if (i == 0) {
                for (int j = 0; j < n; ++j)
                    perm[j] = j;
                return *this;
            }
            if (perm[i] > perm[i - 1]) {
                for (int j = i; j < (n + i) / 2; ++j)
                    swap(j, n - 1 - j + i);
                int k = -1;
                for (int j = i; j < n; ++j)
                    if (perm[j] > perm[i - 1] && (k == -1 || perm[j] < perm[k]))
                        k = j;
                swap(i - 1, k);
                return *this;
            }
        }
        return *this;
    }

    const Permutation operator++(int) {
        Permutation p(n, perm);
        ++(*this);
        return p;
    }

    template<std::size_t N, typename T>
    void operator() (T (&begin)[N]) const {
        assert(n == N);
        T* tmp = new T[n];
        for (int i = 0; i < n; ++i)
            tmp[perm[i]] = begin[i];
        for (int i = 0; i < n; ++i)
            begin[i] = tmp[i];
        delete[] tmp;
        return;
    }

    const Permutation previous() const {
        Permutation p(n, perm);
        --p;
        return p;
    }

    Permutation& operator--() {
        for (int i = 0; i < n; ++i)
            perm[i] = n - 1 - perm[i];
        ++(*this);
        for (int i = 0; i < n; ++i)
            perm[i] = n - 1 - perm[i];
        return *this;
    }

    const Permutation operator--(int) {
        Permutation p(n, perm);
        --(*this);
        return p;
    }

    bool operator==(const Permutation& p) const {
        if (n != p.n)
            return false;
        for (int i = 0; i < n; ++i)
            if (perm[i] != p[i])
                return false;
        return true;
    }

    bool operator!=(const Permutation& p) const {
        return !(*this == p);
    }

    bool operator<(const Permutation& p) const {
        if (n < p.n)
            return true;
        if (n > p.n)
            return false;
        for (int i = 0; i < n; ++i) {
            if (perm[i] < p[i])
                return true;
            else if (perm[i] > p[i])
                return false;
        }
        return false;
    }

    bool operator<=(const Permutation& p) const {
        return *this < p || *this == p;
    }

    bool operator>(const Permutation& p) const {
        return !(*this <= p);
    }

    bool operator>=(const Permutation& p) const {
        return !(*this < p);
    }

    const Permutation inverse() const {
        int p = 0;
        int* sizes = new int[n];
        int** m = buildCycles(p, sizes);
        Permutation ans(n);
        for (int i = 0; i < p; ++i) {
            int k = sizes[i];
            for (int j = 0; j < k / 2; ++j) {
                m[i][j] += m[i][k - 1 - j];
                m[i][k - 1 - j] = m[i][j] - m[i][k - 1 - j];
                m[i][j] -= m[i][k - 1 - j];
            }
            for (int j = 0; j < k; ++j)
                ans.perm[m[i][j]] = m[i][(j + 1) % k];
        }
        for (int i = 0; i < p; ++i)
            delete[] m[i];
        delete[] sizes;
        return ans;
    }
    
    size_t derangementsCount() const {
        int* buffer = new int[n];
        for (int i = 0; i < n; ++i)
            buffer[i] = perm[i];
        size_t ans = derangements(buffer, buffer + n, buffer);
        delete[] buffer;
        return ans;
    }

    int** buildCycles(int& p, int* sizes) const {
        p = 0;
        int** ans = new int* [n];
        bool* tmp = new bool[n];
        for (int i = 0; i < n; ++i)
            tmp[i] = false;
        for (int i = 0; i < n; ++i)
            if (!tmp[i]) {
                int x = 0;
                int y = 0;
                int j = i;
                while (!tmp[j]) {
                    tmp[j] = true;
                    ++x;
                    j = perm[j];
                }
                sizes[p] = x;
                ans[p++] = new int[x];
                do {
                    ans[p - 1][y++] = j;
                    j = perm[j];
                } while (y < x);
            }
        delete[] tmp;
        return ans;
    }

    bool parity() const {
        int* sizes = new int [n];
        int p = 0;
        buildCycles(p, sizes);
        bool ans = 0;
        for (int i = 0; i < p; ++i)
            ans ^= (sizes[i] - 1) % 2;
        delete[] sizes;
        return ans;
    }

    bool isOdd() const {
        return parity();
    }

    bool isEven() const {
        return !parity();
    }

    const Permutation pow(const int& degree) const {
        int p = 0;
        int* sizes = new int [n];
        int** m = buildCycles(p, sizes);
        Permutation ans(n);
        for (int i = 0; i < p; ++i) {
            int k = sizes[i];
            for (int j = 0; j < k; ++j) {
                int tmp = j + degree;
                ans.perm[m[i][j]] = m[i][(tmp >= 0 ? tmp % k : (k + (tmp % k)) % k)];
            }
        }
        for (int i = 0; i < p; ++i)
            delete[] m[i];
        delete[] sizes;
        return ans;
    }
};