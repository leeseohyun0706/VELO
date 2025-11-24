#ifndef __VELO_OPERATOR
#define __VELO_OPERATOR

namespace VELO {

    template<typename _Arg1, typename _Arg2, typename _Result>
    struct __operator_binary {
        using Arg1Type = _Arg1;
        using Arg2Type = _Arg2;
        using ResultType = _Result;
    };

    template<typename _Arg1, typename _Arg2, typename _Result>
    struct __operator_unary {
        using Arg1Type = _Arg1;
        using Arg2Type = _Arg2;
        using ResultType = _Result;
    };

    template<typename T>
    struct __compare_less
        : __operator_binary<T, T, bool>
    {
        bool operator()(const T& lhs, const T& rhs) { return lhs < rhs; }
    };

    template<typename T>
    struct __compare_less_equal
        : __operator_binary<T, T, bool>
    {
        bool operator()(const T& lhs, const T& rhs) { return lhs <= rhs; }
    };
    template<typename T>
    struct __compare_more
        : __operator_binary<T, T, bool>
    {
        bool operator()(const T& lhs, const T& rhs) { return lhs > rhs; }
    };
    
    template<typename T>
    struct __compare_more_equal
        : __operator_binary<T, T, bool>
    {
        bool operator()(const T& lhs, const T& rhs) { return lhs >= rhs; }
    };

    template<typename T>
    struct __compare_equal
        : __operator_binary<T, T, bool>
    {
        bool operator()(const T& lhs, const T& rhs) { return lhs == rhs; }
    };

    template<typename T>
    struct __compare_not_equal
        : __operator_binary<T, T, bool>
    {
        bool operator()(const T& lhs, const T& rhs) { return lhs != rhs; }
    };
    
}

#endif