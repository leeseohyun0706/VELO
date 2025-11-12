#ifndef __VELO_TUPLE
#define __VELO_TUPLE

#include <type_traits>
#include <utility>
namespace VELO {

    template<typename ...Types>
    class __tuple;

    template<typename HeadType, typename ...TailTypes>
    class __tuple<HeadType, TailTypes...> {
        HeadType head;
        __tuple<TailTypes...> tail;
        
        public:

        __tuple() {}
        __tuple(HeadType const&  head, __tuple<TailTypes...> const & tail) : head(head), tail(tail) {}

        template<typename VHead, typename ...VTails, typename=std::enable_if_t<sizeof...(VTails) == sizeof...(TailTypes)>>
        __tuple(VHead&& h, VTails&&... tails) : head(std::forward<VHead>(h)), tail(std::forward<VTails>(tails)...) {}

        template<typename VHead, typename ...VTails, typename=std::enable_if_t<sizeof...(VTails) == sizeof...(TailTypes)>>
        __tuple(const __tuple<VHead, VTails...>& other) : head(other.getHead()), tail(other.getTail()) {}

        HeadType& getHead() { return head; }
        const HeadType& getHead() const { return head; }
        auto& getTail() { return tail; }
        const auto& getTail() const { return tail; }
    };

    template<>
    class __tuple<> {};

    template<unsigned N>
    class __tuple_get {
        public:
        template<typename Head, typename ...Tails>
        static const auto& apply(const __tuple<Head, Tails...> &t) {
            return __tuple_get<N-1>::apply(t.getTail());
        }
    };

    template<>
    class __tuple_get<0> {
        public:
        template<typename Head, typename ...Tails>
        static const auto& apply(const __tuple<Head, Tails...>& t) {
            return t.getHead();
        }
    };

    template<unsigned N, typename ...Types>
    inline const auto& get(const __tuple<Types...>& t) {
        return __tuple_get<N>::apply(t);
    }
    
    template<typename Head, typename ...Tails>
    using Tuple = __tuple<Head, Tails...>;

    template<typename ...Types>
    auto makeTuple(Types&&... elm) {
        return __tuple<std::decay_t<Types>...>(std::forward<Types>(elm)...);
    }
}
#endif 