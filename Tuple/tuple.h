#include <iostream>
#include <cassert>
#include <cstring>


template <std::size_t Index, typename T>
struct TupleElement {
    T value_;
    template <typename U>
    explicit TupleElement(U&& value) : value_(std::forward<U>(value)) {}

    TupleElement() = default;
    TupleElement(const TupleElement&) = default;
    TupleElement(TupleElement&&) = default;
    TupleElement& operator=(const TupleElement&) = default;
    TupleElement& operator=(TupleElement&&) = default;
};


template <std::size_t... Indices>
struct IndexSequence {
    using type = IndexSequence<Indices...>;
};


template <std::size_t LastIndex, typename Sequence>
struct CatIndexSequence;


template <std::size_t LastIndex, std::size_t... Indices>
struct CatIndexSequence<LastIndex, IndexSequence<Indices...> >
    : IndexSequence<Indices..., LastIndex> {}; 


template <std::size_t N>
struct MakeIndexSequence
    : CatIndexSequence<N - 1, typename MakeIndexSequence<N - 1>::type> {};


template <>
struct MakeIndexSequence<1> : IndexSequence<0> {};

template <>
struct MakeIndexSequence<0> : IndexSequence<> {};


template <typename Sequence, typename... Types>
struct TupleImpl;


template <std::size_t... Indices, typename... Types>
struct TupleImpl<IndexSequence<Indices...>, Types...> : TupleElement<Indices, Types>... {
    template <typename... UTypes>
    explicit TupleImpl(UTypes&&... args)
        : TupleElement<Indices, Types>(std::forward<UTypes>(args))... {}

    TupleImpl() = default;
    TupleImpl(const TupleImpl&) = default;
    TupleImpl(TupleImpl&&) = default;
    TupleImpl& operator=(const TupleImpl&) = default;
    TupleImpl& operator=(TupleImpl&&) = default;
};


template <typename... Types>
class Tuple : public TupleImpl<typename MakeIndexSequence<sizeof...(Types)>::type, Types...> {
private:
    using TupleImplType =
        TupleImpl<typename MakeIndexSequence<sizeof...(Types)>::type, Types...>;

public:
    Tuple() = default;
    Tuple(const Tuple&) = default;
    Tuple(Tuple&&) = default;
    Tuple& operator=(const Tuple&) = default;
    Tuple& operator=(Tuple&&) = default;

    template <typename... UTypes>
    explicit Tuple(const Tuple<UTypes...>& another) : TupleImplType(another) {}

    template <typename... UTypes>
    explicit Tuple(Tuple<UTypes...>&& another) : TupleImplType(std::move(another)) {}

    template <typename... UTypes, typename = typename std::enable_if<sizeof...(UTypes) == sizeof...(Types)>::type>
    explicit Tuple(UTypes&&... args) : TupleImplType(std::forward<UTypes>(args)...) {}

    void swap(Tuple& another) {
        std::swap(*this, another);
    }
};

template <typename... Types>
Tuple<Types...> makeTuple(Types... args) {
    return Tuple<Types...>(args...);
}

template <std::size_t Index, typename Head, typename... Tail>
struct TypeAtIndex {
    using type = typename TypeAtIndex<Index - 1, Tail...>::type;
};

template <typename Head, typename... Tail>
struct TypeAtIndex<0, Head, Tail...> {
    using type = Head;
};

template <std::size_t N, typename... Types>
const typename TypeAtIndex<N, Types...>::type&  get(const Tuple<Types...>& tuple) {
    const TupleElement<N, typename TypeAtIndex<N, Types...>::type>& te = tuple;
    return te.value_;
}

template <std::size_t N, typename... Types>
typename TypeAtIndex<N, Types...>::type&  get(Tuple<Types...>& tuple) {
    TupleElement<N, typename TypeAtIndex<N, Types...>::type>& te = tuple;
    return te.value_;
}

template <std::size_t N, typename... Types>
std::remove_reference_t<typename TypeAtIndex<N, Types...>::type>&&  get(Tuple<Types...>&& tuple) {
    TupleElement<N, typename TypeAtIndex<N, Types...>::type>&& te = std::move(tuple);
    return std::move(te.value_);
}

template <typename>
constexpr int find(int) {
    return -1;
}

template <typename T, typename Head, typename... Tail>
constexpr int find(int current_index = 0) {
    return std::is_same<T, Head>::value ? current_index
            : find<T, Tail...>(current_index + 1);
}

template <typename T, typename... Types>
const std::remove_reference_t<T>& get(const Tuple<Types...>& tuple) {
    return get<find<T, Types...>()>(tuple);
}

template <typename T, typename... Types>
std::remove_reference_t<T>& get(Tuple<Types...>& tuple) {
    return get<find<T, Types...>()>(tuple);
}

template <typename T, typename... Types>
std::remove_reference_t<T>&& get(Tuple<Types...>&& tuple) {
    return get<find<T, Types...>()>(std::move(tuple));
}

template <typename>
struct TupleSize;

template <typename... Types>
struct TupleSize<Tuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

template <typename Tuple1, size_t... Indices1, typename Tuple2, size_t... Indices2>
auto cat2Tuples_(Tuple1&& tuple1, Tuple2&& tuple2,
                IndexSequence<Indices1...>, IndexSequence<Indices2...>) {
    return makeTuple(get<Indices1>(std::forward<Tuple1>(tuple1))...,
                    get<Indices2>(std::forward<Tuple2>(tuple2))...);
}

template <typename Tuple1, typename Tuple2>
auto cat2Tuples(Tuple1&& tuple1, Tuple2&& tuple2) {
    return cat2Tuples_(std::forward<Tuple1>(tuple1),
                    std::forward<Tuple2>(tuple2),
                    MakeIndexSequence<TupleSize<std::decay_t<Tuple1>>::value>{},
                    MakeIndexSequence<TupleSize<std::decay_t<Tuple2>>::value>{});
}

template <typename HeadTuple>
HeadTuple&& tupleCat(HeadTuple&& tuple) {
    return std::forward<HeadTuple>(tuple);
}

template <typename Head1Tuple, typename Head2Tuple, typename... TailTuples>
auto tupleCat(Head1Tuple&& tuple1, Head2Tuple&& tuple2, TailTuples&&... tail) {
    return tupleCat(cat2Tuples(std::forward<Head1Tuple>(tuple1),
                            std::forward<Head2Tuple>(tuple2)),
                    std::forward<TailTuples>(tail)...);
}

template<std::size_t N>
Tuple<> getTail_() {
    return makeTuple();
}

template<std::size_t N, typename Arg1, typename... Args, typename = typename std::enable_if<N == sizeof...(Args) + 1>::type>
Tuple<Arg1, Args...> getTail_(Arg1 arg1, Args... args) {
    return makeTuple(arg1, args...);
}

template<std::size_t N, typename Arg1, typename... Args, typename = typename std::enable_if<N != sizeof...(Args) + 1>::type>
auto getTail_(Arg1, Args... args) {
    return getTail_<N>(args...);
}

template <std::size_t N, typename Tuple, size_t... Indices>
auto getTail__(Tuple&& tup, IndexSequence<Indices...>) {
    return getTail_<N>(get<Indices>(std::forward<Tuple>(tup))...);
}

template <std::size_t N, typename Tuple>
auto getTail(Tuple&& tuple){
    return getTail__<N>(std::forward<Tuple>(tuple),
            MakeIndexSequence<TupleSize<std::decay_t<Tuple>>::value>{});
}

template <typename Tuple1, typename Tuple2>
typename std::enable_if<TupleSize<std::decay_t<Tuple1>>::value == 0 || TupleSize<std::decay_t<Tuple2>>::value == 0, bool>::type 
operator<(Tuple1&&, Tuple2&&) {
    auto s1 = TupleSize<std::decay_t<Tuple1>>::value, s2 = TupleSize<std::decay_t<Tuple2>>::value;
    return s1 < s2;
}

template <typename Tuple1, typename Tuple2>
typename std::enable_if<!(TupleSize<std::decay_t<Tuple1>>::value == 0 || TupleSize<std::decay_t<Tuple2>>::value == 0), bool>::type 
operator<(Tuple1&& tuple1, Tuple2&& tuple2) {
    auto a = get<0>(tuple1);
    auto b = get<0>(tuple2);
    return a < b ? true : (a > b ? false : getTail<TupleSize<std::decay_t<Tuple1>>::value - 1>(tuple1) < getTail<TupleSize<std::decay_t<Tuple2>>::value - 1>(tuple2));
}

template <typename Tuple1, typename Tuple2>
typename std::enable_if<TupleSize<std::decay_t<Tuple1>>::value == 0 || TupleSize<std::decay_t<Tuple2>>::value == 0, bool>::type 
operator==(Tuple1&&, Tuple2&&) {
    auto s1 = TupleSize<std::decay_t<Tuple1>>::value, s2 = TupleSize<std::decay_t<Tuple2>>::value;
    return s1 == s2;
}

template <typename Tuple1, typename Tuple2>
typename std::enable_if<!(TupleSize<std::decay_t<Tuple1>>::value == 0 || TupleSize<std::decay_t<Tuple2>>::value == 0), bool>::type 
operator==(Tuple1&& tuple1, Tuple2&& tuple2) {
    auto a = get<0>(tuple1);
    auto b = get<0>(tuple2);
    return a != b ? false : getTail<TupleSize<std::decay_t<Tuple1>>::value - 1>(tuple1) == getTail<TupleSize<std::decay_t<Tuple2>>::value - 1>(tuple2);
}

template <typename Tuple1, typename Tuple2>
bool operator!=(Tuple1&& tuple1, Tuple2&& tuple2) {
    return !(tuple1 == tuple2);
}

template <typename Tuple1, typename Tuple2>
bool operator<=(Tuple1&& tuple1, Tuple2&& tuple2) {
    return tuple1 < tuple2 || tuple1 == tuple2;
}

template <typename Tuple1, typename Tuple2>
bool operator>(Tuple1&& tuple1, Tuple2&& tuple2) {
    return !(tuple1 <= tuple2);
}

template <typename Tuple1, typename Tuple2>
bool operator>=(Tuple1&& tuple1, Tuple2&& tuple2) {
    return !(tuple1 < tuple2);
}
