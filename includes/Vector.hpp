#ifndef __VELO_VECTOR
#define __VELO_VECTOR

#include "Allocator.hpp"
#include "IteratorBase.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <initializer_list>

namespace VELO {
    template<typename T, typename _Alloc = Basic_Template_Allocator<T> >
    class Vector {
        private:
        size_t cap{};
        size_t sz{};
        T* ele = nullptr;
        _Alloc __alloc;

        inline _Alloc& _alloc() { return __alloc; }
        template<typename IT>
        class VectorIterator : public Iterator<IT> {
            IT* cur = nullptr;
            public:
            VectorIterator(IT* cur) : cur(cur) {}
            IT& operator*() { return *cur; }
            VectorIterator& operator++() { cur++; return *this; }
            bool operator==(const VectorIterator& iter) { return iter.cur == cur; }
            bool operator!=(const VectorIterator& iter) { return iter.cur != cur; }
        };
        
        using Pointer = T*;
        using Reference = T&;
        using ConstType = const T;
        using ConstPointerType = const T*;
        using ConstReferenceType = const T&;

        using IterType = VectorIterator<T>;
        using ConstIterType = VectorIterator<ConstType>;
        
        size_t _find_idx(const T& val) const {
            for(size_t i = 0; i < cap; ++i) {
                if(ele[i] == val) return i;
            }
            return SIZE_MAX;
        }
        

        public:
        
        Vector() { reserve(7); }
        Vector(std::initializer_list<T> ilist) { reserve(ilist.size()); sz=ilist.size(); copy(ilist.begin(), ilist.end(), begin()); }
        Vector(size_t s) { reserve(s); }
        Vector(const Vector& vec) { reserve(vec.cap); sz=vec.sz; copy(vec.begin(), vec.end(), this->begin()); }

        Vector(const _Alloc& allocator) : __alloc(allocator) { reserve(7); }
        Vector(size_t s, const _Alloc& allocator) : __alloc(allocator) { this->Vector(s); }
        Vector(const Vector& vec, const _Alloc& allocator) : __alloc(allocator) { reserve(vec.cap); sz=vec.size; copy(vec.cbegin(), vec.cend(), this->begin()); } 
        
        template <class _TIter>
        Vector(_TIter begin, _TIter end) { reserve(7); while(begin != end) { push_back(*begin); ++begin; } }

        template <class _TIter>
        Vector(_TIter begin, _TIter end, const _Alloc allocator) : __alloc(allocator) { reserve(7); while(begin != end) { push_back(*begin); ++begin; } }
        ~Vector() {
            cap=0; sz=0;
            __alloc.dealloc(ele);
        }

        void reserve(size_t s) {
            if(cap > s) return;
            cap = s;
            auto tmp = __alloc.alloc(s);
            if(ele) __alloc.dealloc(ele);
            ele = tmp;
            std::memset((void*)ele, 0, sizeof(T)*cap);
            sz = 0;
        }
        void resize(size_t s) {
            cap = s;
            auto tmp = __alloc.alloc(s);
            std::memcpy((void*)tmp, (void*)ele, sz*sizeof(T));
            if(ele) __alloc.dealloc(ele);
            ele = tmp;
        }
        void push_back(const T& v) {
            if(sz+1 > cap) resize(cap*2);
            T* slot = ele+sz;
            new (slot) T(v);
            ++sz;
        }
        void pop_back() {
            if(sz < 1) return;
            --sz;
        }

        inline T& top() {
            assert(!empty());
            return ele[sz-1];
        }
        inline T& bottom() { return ele[0]; }
        inline T& at(size_t index) { return ele[index]; } 
        inline const T& at(size_t index) const { return ele[index]; }
        inline size_t size() const { return sz; }
        inline size_t capacity() const { return cap; }
        inline void clear() { sz=0; }

        inline T& operator[](size_t index) { return at(index); }

        inline IterType begin() { return IterType(&at(0)); }
        inline IterType end() { return IterType(&at(sz)); }
        inline ConstIterType begin() const { return ConstIterType(&at(0)); }
        inline ConstIterType end() const { return ConstIterType(&at(sz)); }

        inline IterType find(const T& val) { size_t idx = _find_idx(val); if(idx == SIZE_MAX) return end(); else return IterType(&at(idx)); }
        inline ConstIterType find(const T& val) const { size_t idx = _find_idx(val); if(idx == SIZE_MAX) return end(); else return ConstIterType(&at(idx)); }
        inline bool empty() const { return sz == 0; }

        void swap(Vector& vec) {
            std::swap(cap, vec.cap);
            std::swap(sz, vec.sz);
            std::swap(ele, vec.ele);
            std::swap(__alloc, vec.__alloc);
        }

        void operator=(const Vector& other) {
            reserve(other.cap);
            copy(other.begin(), other.end(), begin());
            sz = other.sz;
        }
    };
}

#endif 
