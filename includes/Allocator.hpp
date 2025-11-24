#ifndef __VELO_ALLOCATOR
#define __VELO_ALLOCATOR


#include <cstddef>
namespace VELO {

    template<typename T>
    class Allocator {
        using Pointer = T*;
        public:
        
        virtual Pointer alloc(size_t n) = 0;
        virtual void dealloc(Pointer p) = 0;
    };

    template<typename T>
    class Basic_Template_Allocator : public Allocator<T> {
        using Pointer = T*;
        public:

        Pointer alloc(size_t n) {
            return (Pointer)(::operator new(n * sizeof(T)));
        }

        void dealloc(Pointer p) {
            delete (char*)p;
        }

        void constructor(Pointer p, const T& value) {
            new ((void*)p) T(value);
        }

        void destory(Pointer p) {
            p->~T();
        }
    };

}


#endif 