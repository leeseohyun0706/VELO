#ifndef __VELO_ITERATOR
#define __VELO_ITERATOR

#include "IteratorBase.hpp"
#include <functional>
#include <utility>
namespace VELO {
    
    template <class _SIter, class _OIter>
    void copy(_SIter begin, _SIter end, _OIter out) {
        while(begin != end) {
            *out = *begin;
            ++out;
            ++begin;
        }
    }
    
    template<class IterType, class FuncType>
    void for_each(IterType first, IterType last, FuncType f) {
        while(first != last) {
            f(*first);
            ++first;
        }
    }
    
    template<class IterType, class FuncType>
    IterType find_if(IterType first, IterType last, FuncType f) {
        while(first != last) {
            if(f(*first)) return first;
            ++first;
        }
        return last;
    }

    template<class IterType>
    class __iterable {
        IterType first;
        IterType last;

        public:
        explicit __iterable(IterType f, IterType l) : first(f), last(l) {}

        template<class FP>
        inline __iterable& for_each(FP func) {
            VELO::for_each(first, last, func);
            return *this;
        }


    };


}

#endif
