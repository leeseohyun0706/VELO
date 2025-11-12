#ifndef __VELO_ARRAY
#define __VELO_ARRAY

#include "Allocator.hpp"
#include "Vector.hpp"
#include <cstdint>
namespace VELO {

    template<typename T, size_t sz>
    class __array {
        T arr[sz]{};

        template<typename T1>
        class __array_iterator : Iterator<T1> {
            T1* cur{nullptr};
            
            public:

            explicit __array_iterator(T1* cur) : cur(cur) {}
            inline T1& operator*() { return *cur; }
            
            inline __array_iterator& operator++() { ++cur; return *this; }
            
            inline bool operator==(const __array_iterator& other) { return cur == other.cur; }
            inline bool operator!=(const __array_iterator& other) { return !(cur == other.cur); }
        };

        using IterType = __array_iterator<T>;
        using ConstIterType = __array_iterator<const T>;

        size_t _find_idx(const T& val) const { for(size_t i = 0; i < sz; ++i) { if(arr[i] == val) { return i; } } return SIZE_MAX; }
        
        public:
        
        __array() {}
        
        inline size_t size() const { return sz; }
        inline T* data() { return arr; }
        inline T& at(size_t idx) { return arr[idx]; }
        inline const T& at(size_t idx) const { return arr[idx]; }
        
        inline T& operator[](size_t idx) { return at(idx); }
        inline const T& operator[](size_t idx) const { return at(idx); }

        inline IterType begin() { return IterType(&at(0)); }
        inline IterType end() { return IterType(&at(sz)); } 
        inline ConstIterType begin() const { return ConstIterType(&at(0)); }
        inline ConstIterType end() const { return ConstIterType(&at(sz)); }

        inline IterType find(const T& val) { return IterType(&at(_find_idx(val))); }
        inline ConstIterType find(const T& val) const { return ConstIterType(&at(_find_idx(val))); }
    };


    template <typename T, size_t sz>
    using FixedArray = __array<T, sz>;
}

#endif