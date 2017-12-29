#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include <set>
#include <vector>
#include <cassert>


class SuffixAutomaton {
private:
    struct SAVertex {
        std::size_t len;
        SAVertex* link;
        std::map<char, SAVertex*> next;
        bool terminal;

        std::size_t inclusionNumber;
        char refrainLetter;
        bool refrainVertexIsFound;
        bool parentIsFound;
        SAVertex* longestParent;
        SAVertex* refrainVertex;


        SAVertex() : len(0), link(nullptr), inclusionNumber(0),
                     refrainVertexIsFound(false), longestParent(nullptr),
                     parentIsFound(false), refrainVertex(nullptr), terminal(false) {}
        

        ~SAVertex() {
            if (link)
                delete link;
        }
    };



    SAVertex* start_;
    SAVertex* last_;



    void buildAutomaton_(const std::string& str) {
        for (const char& c : str) {
            SAVertex* newVertex = new SAVertex();
            newVertex->len = 1 + last_->len;

            SAVertex* vertex = last_;
            for (; vertex && !vertex->next[c]; vertex = vertex->link)
                vertex->next[c] = newVertex;

            if (!vertex)
                newVertex->link = start_;
            else {
                SAVertex* nextVertex = vertex->next[c];
                if (vertex->len + 1 == nextVertex->len)
                    newVertex->link = nextVertex;
                else {
                    SAVertex* clone = new SAVertex();
                    
                    clone->len = vertex->len + 1;
                    clone->next = nextVertex->next;
                    clone->link = nextVertex->link;

                    for (; vertex && vertex->next[c] == nextVertex; vertex = vertex->link)
                        vertex->next[c] = clone;
                    nextVertex->link = newVertex->link = clone;
                }
            }
            last_ = newVertex;
        }
    }


    void findTerminalStates_() {
        for (SAVertex* current = last_; current != start_ && current; current = current->link)
            current->terminal = true;
    } 


    void countInclusionNumbersDFS_(SAVertex* vertex) {
        for (auto& pair : vertex->next) {
            if (!pair.second->inclusionNumber)
                countInclusionNumbersDFS_(pair.second);
            vertex->inclusionNumber += pair.second->inclusionNumber;
        }
        if (vertex->terminal)
            ++vertex->inclusionNumber;
    }


    void findLongestParents_(SAVertex* vertex) {
        vertex->parentIsFound = true;
        for (auto& pair : vertex->next) {
            if (!pair.second->parentIsFound)
                findLongestParents_(pair.second);
            if (!pair.second->longestParent || pair.second->longestParent->len < vertex->len) {
                pair.second->longestParent = vertex;
                pair.second->refrainLetter = pair.first;
            }
        }
            
    }


    SAVertex* findRefrainVertexDFS_(SAVertex* subtreeRoot) {
        if (subtreeRoot->refrainVertexIsFound)
            return subtreeRoot->refrainVertex;
        long long currentRefrainValue = static_cast<long long>(subtreeRoot->len) *
                                        static_cast<long long>(subtreeRoot->inclusionNumber);
        SAVertex* ans = subtreeRoot;
        for (auto& pair : subtreeRoot->next) {
            SAVertex* refrainVertex1 = findRefrainVertexDFS_(pair.second);
            long long tmp = static_cast<long long>(refrainVertex1->inclusionNumber) *
                            static_cast<long long>(refrainVertex1->len);
            if (tmp > currentRefrainValue) {
                currentRefrainValue = tmp;
                ans = refrainVertex1;
            }
        }
        subtreeRoot->refrainVertexIsFound = true;
        return subtreeRoot->refrainVertex = ans;
    }


public:
    SuffixAutomaton(const std::string& str) 
        : start_(new SAVertex()), last_(start_) {
            buildAutomaton_(str);
            findTerminalStates_();
            countInclusionNumbersDFS_(start_);
            findLongestParents_(start_);
        }


    ~SuffixAutomaton() {
        if (start_)
            delete start_;
    }


    std::pair<std::string, long long> getRefrain() {
        SAVertex* refrainVertex1 = findRefrainVertexDFS_(start_);
        std::string ans;
        for (SAVertex* i = refrainVertex1; i->longestParent; i = i->longestParent)
            ans += i->refrainLetter;
        std::reverse(ans.begin(), ans.end());
        return {ans, refrainVertex1->inclusionNumber};
    }
};




class SuffixTree {
private:
    static const unsigned INF = 1e9;
    static const char STOP_SYMBOL = '@';
    

    struct STVertex {
        std::size_t first, len, strLen;
        STVertex *link;
        std::map<char, STVertex*> next;

        std::size_t inclusionNumber;


        STVertex() = delete;


        STVertex(const std::size_t& first, const std::size_t& len)
            : first(first), len(len), link(nullptr), inclusionNumber(0), strLen(0) {}


        ~STVertex() {
            if (link)
                delete link;
        }
    };



    const std::string str_;
    STVertex* start_;



    void buildTree_() {
        STVertex* longestNonUniqueSuffix = start_;
        std::size_t pos = 0;
        
        for (int i = 0; i < str_.size(); ++i) {
            ++pos;
            STVertex* last = start_;
            while (pos) {
                while (longestNonUniqueSuffix->next[str_[i + 1 - pos]] &&
                       pos > longestNonUniqueSuffix->next[str_[i + 1 - pos]]->len) {
                    longestNonUniqueSuffix = longestNonUniqueSuffix->next[str_[i + 1 - pos]];
                    pos -= longestNonUniqueSuffix->len;
                }

                STVertex* vertex = longestNonUniqueSuffix->next[str_[i + 1 - pos]] ? 
                        longestNonUniqueSuffix->next[str_[i + 1 - pos]] : start_;

                char ch = str_[(vertex ? vertex->first : 0) + pos - 1];

                if (vertex == start_) {
                    STVertex* newVertex = new STVertex(i + 1 - pos, INF);
                    last->link = longestNonUniqueSuffix;
                    last = start_;
                    longestNonUniqueSuffix->next[str_[i + 1 - pos]] = newVertex;
                }
                else if (ch == str_[i]) {
                    last->link = longestNonUniqueSuffix;
                    break;
                }
                else  {
                    STVertex* newVertex = new STVertex(vertex->first, pos - 1);
                    newVertex->next[str_[i]] = new STVertex(i, INF);
                    newVertex->next[ch] = vertex;
                    vertex->first += pos - 1;
                    vertex->len -= pos - 1;
                    longestNonUniqueSuffix->next[str_[i + 1 - pos]] = newVertex;
                    last->link = newVertex;
                    last = newVertex;
                }
                if (longestNonUniqueSuffix == start_)
                    --pos;
                else
                    longestNonUniqueSuffix = longestNonUniqueSuffix->link;
            }
        }
    }


    void findStrLenDFS_(STVertex* vertex) {
        for (auto& pair : vertex->next)
            if (!pair.second->next.size())
                pair.second->strLen = vertex->strLen + (str_.size() - pair.second->first - 1);
            else {
                pair.second->strLen = vertex->strLen + pair.second->len;
                findStrLenDFS_(pair.second);
            }
    }


    void countInclusionNumbersDFS_(STVertex* vertex) {
        for (auto& pair : vertex->next) {
            countInclusionNumbersDFS_(pair.second);
            vertex->inclusionNumber += pair.second->inclusionNumber;
        }
        if (!vertex->inclusionNumber)
            vertex->inclusionNumber = 1;
    }


    STVertex* findRefrainVertexDFS_(STVertex* vertex) const {
        STVertex* ans = vertex;
        for (auto& pair : vertex->next) {
            STVertex* tmp = findRefrainVertexDFS_(pair.second);
            if (static_cast<long long>(tmp->strLen) * static_cast<long long>(tmp->inclusionNumber) > 
                static_cast<long long>(ans->strLen) * static_cast<long long>(ans->inclusionNumber))
                ans = tmp;
        }
        return ans;
    }



public:
    SuffixTree(const std::string& str) 
        : str_(str + STOP_SYMBOL), start_(new STVertex(0, INF)) {
            buildTree_();
            findStrLenDFS_(start_);
            countInclusionNumbersDFS_(start_);
        }


    ~SuffixTree() {
        if (start_)
            delete start_;
    }


    std::pair<std::string, long long> getRefrain() const {
        STVertex* refrainVertex = findRefrainVertexDFS_(start_);
        return {refrainVertex->next.size() ? 
            str_.substr(refrainVertex->first + refrainVertex->len - refrainVertex->strLen, refrainVertex->strLen) :
            str_.substr(str_.size() - refrainVertex->strLen - 1, refrainVertex->strLen), refrainVertex->inclusionNumber};
    }
};


class SuffixArray {
private:
    const static std::size_t ALPHABET_SIZE = 256;
    const static char MINIMAL_CHAR = '$';

    const std::string str_;
    const std::vector<std::size_t> array_;


    std::vector<std::size_t> sortSuffixShifts_(const std::string& str) {
        std::size_t n = str.size();
        std::vector<std::size_t> shifts(n), newShifts(n), colors(n), newColors(n), h;

        h.assign(ALPHABET_SIZE, 0);
        for (int i = 0; i < n; ++i) {
            colors[i] = str[i];
            ++h[colors[i]];
        }
        for (int i = 1; i < ALPHABET_SIZE; ++i)
            h[i] += h[i - 1];
        for (int i = n - 1; i >= 0; --i)
            newShifts[--h[colors[i]]] = i;
        shifts.swap(newShifts);

        for (int length = 1; length <= n; length += length) {
            h.assign(ALPHABET_SIZE, 0);
            for (int i = 0; i < n; ++i)
                ++h[colors[i]];
            for (int i = 1; i < ALPHABET_SIZE; ++i)
                h[i] += h[i - 1];
            for (int i = n - 1; i >= 0; --i) {
                int first = (shifts[i] - length + n) % n;
                newShifts[--h[colors[first]]] = first;
            }

            shifts.swap(newShifts);

            std::size_t lastColor = 1;
            newColors[shifts[0]] = 0;
            for (int i = 1; i < n; ++i)
                if (colors[shifts[i]] == colors[shifts[i - 1]] &&
                    colors[(shifts[i] + length) % n] == colors[(shifts[i - 1] + length) % n])
                    newColors[shifts[i]] = lastColor - 1;
                else
                    newColors[shifts[i]] = lastColor++;
            colors.swap(newColors);
        }
        shifts.erase(shifts.begin());
        return shifts;
    }


public:
    SuffixArray(const std::string& str)
        : str_(str), array_(sortSuffixShifts_(str + MINIMAL_CHAR)) {}


    const std::size_t operator[](const std::size_t index) const {
        return array_[index];
    }


    const std::size_t size() const {
        return str_.size();
    }


    const std::string getString() const {
        return str_;
    }
};
