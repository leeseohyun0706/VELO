#ifndef __VELO_HASHMAP
#define __VELO_HASHMAP
#include "Allocator.hpp"
#include "Hash.hpp"
#include "IteratorBase.hpp"
#include <algorithm>
#include <cstddef>
#include <vector>

namespace VELO {
    
    
    template <typename K, typename V, typename HashType = size_t>
    class __hash_map_node {
        using KeyType = K;
        using ValueType = V;
        using ConstKeyType = const K;
        using ConstValueType = const V;
        using RefValueType = V&;
        using ConstRefValueType = const V&;
        HashType hash;
        K key;
        V val;
        class __hash_map_node* next{nullptr};
        public:
        explicit __hash_map_node(HashType hash, const K& k, const V& v, class __hash_map_node* n) : hash(hash), key(k), val(v), next(n) {}
        
        inline K& key_data() { return key; }
        inline const K& key_data() const { return key; }

        inline V& val_data() { return val; }
        inline const V& val_data() const { return val; }

        inline HashType& hash_data() { return hash; }
        inline const HashType& hash_data() const { return hash; }

        inline class __hash_map_node*& next_data() { return next; }
        
        template <class Alloc>
        inline static __hash_map_node* create_node(HashType hash, const K& k, const V& v, class __hash_map_node* n, Alloc& alloc) {
            auto* vp = alloc.alloc(1);
            new (vp) __hash_map_node(hash, k, v, n);
            return vp;
        }
    };


    template <typename T1, typename T2>
    class __pair {
        public:
        T1 first;
        T2 second;

        constexpr __pair() : first(T1()), second(T2()) {}
        __pair(const T1& f, const T2& s) : first(f), second(s) {}

        ~__pair() = default;
    };
    
    template<typename T1, typename T2>
    using Pair = __pair<T1, T2>;


    template <typename K, typename V, class Hasher = Hash<K>, class Alloc = Basic_Template_Allocator<__hash_map_node<K, V>>>
    class __hash_map {
        using HashNodeType = __hash_map_node<K, V>;
        using HashType = size_t;
        using HashNodePair = __pair<K, V>;
        using BucketType = std::vector<HashNodeType*>;
        static constexpr size_t default_bucket_count = 16;
        
        template<typename K1, typename V1>
        class __hash_map_iterator : public Iterator<__pair<K1, V1>> {
            using RefValueType = __pair<K1&, V1&>;
            HashNodeType* cur{nullptr};
            BucketType& buckets;
            size_t icur{};
            public:

            explicit __hash_map_iterator(HashNodeType* c, BucketType& b, size_t idx) : cur(c), buckets(b), icur(idx) {}

            RefValueType operator*() {
                return RefValueType(cur->key_data(), cur->val_data());
            }
            
            __hash_map_iterator& operator++() {
                if(cur == nullptr) return *this;
                if(cur->next_data() != nullptr) {
                    cur = cur->next_data();
                    return *this;
                }
                auto bucket_count = buckets.size();
                while(++icur < bucket_count) {
                    auto* next = buckets[icur];
                    if(next != nullptr) {
                        cur = next;
                        return *this;
                    }
                }
                cur = nullptr;
                return *this;
            }

            inline bool operator==(const __hash_map_iterator& iter) { return cur == iter.cur; }
            inline bool operator!=(const __hash_map_iterator& iter) { return !(cur == iter.cur); }

        };
        

        Alloc __alloc{};
        BucketType buckets;
        size_t sz{};
        float max_load_factor{0.65f};
        size_t bucket_mask{};
        size_t growth_threshold{};

        static size_t _next_power_of_two(size_t value) {
            if(value <= 1) return 1;
            --value;
            value |= value >> 1;
            value |= value >> 2;
            value |= value >> 4;
            value |= value >> 8;
            value |= value >> 16;
            if constexpr (sizeof(size_t) == 8) {
                value |= value >> 32;
            }
            return value + 1;
        }

        inline size_t _calc_threshold(size_t capacity) const {
            if(capacity == 0) return 0;
            auto threshold = static_cast<size_t>(static_cast<float>(capacity) * max_load_factor);
            return threshold == 0 ? 1 : threshold;
        }

        inline void _prepare_growth_metrics(size_t capacity) {
            bucket_mask = capacity ? (capacity - 1) : 0;
            growth_threshold = _calc_threshold(capacity);
        }

        inline size_t _bucket_index(HashType hash) const {
            return bucket_mask ? (hash & bucket_mask) : 0;
        }

        void _init_buckets(size_t init_size) {
            if(init_size == 0) init_size = 1;
            auto capacity = _next_power_of_two(init_size);
            buckets.assign(capacity, nullptr);
            _prepare_growth_metrics(capacity);
            sz = 0;
        }

        void _destroy_nodes() {
            if(buckets.empty()) return;
            for(size_t i = 0; i < buckets.size(); ++i) {
                HashNodeType* node = buckets[i];
                while(node != nullptr) {
                    HashNodeType* next = node->next_data();
                    __alloc.dealloc(node);
                    node = next;
                }
                buckets[i] = nullptr;
            }
            sz = 0;
        }

        inline HashType _hash(const K& key) { return Hasher()(key); } 
        inline bool _needs_rehash(size_t next_size) const {
            if(buckets.empty()) return true;
            return next_size >= growth_threshold;
        }

        HashNodeType* _find_node(HashType hash, const K& key) {
            if(buckets.empty()) return nullptr;
            auto i = _bucket_index(hash);
            HashNodeType* cur = buckets[i];
            while(cur != nullptr) {
                if(cur->hash_data() == hash && cur->key_data() == key) return cur;
                cur = cur->next_data();
            }
            return nullptr;
        }

        inline const HashNodeType* _find_node(HashType hash, const K& key) const {
            return const_cast<__hash_map*>(this)->_find_node(hash, key);
        }

        void _erase_impl(const K& key) {
            if(buckets.empty()) return;
            auto hash = _hash(key);
            auto i = _bucket_index(hash);
            HashNodeType* cur = buckets[i];
            HashNodeType* prev = nullptr;
            while(cur != nullptr) {
                if(cur->hash_data() == hash && cur->key_data() == key) {
                    if(prev) {
                        prev->next_data() = cur->next_data();
                    }else {
                        buckets[i] = cur->next_data();
                    }
                    __alloc.dealloc(cur);
                    --sz;
                    return;
                }
                prev = cur;
                cur = cur->next_data();
            }
        }

        inline HashNodeType* _find_impl(const K& key) {
            return _find_node(_hash(key), key);
        }

        inline const HashNodeType* _find_impl(const K& key) const {
            return _find_node(_hash(key), key);
        }

        inline HashNodeType* _find_or_insert(const K& key, const V& val={}) {
            auto hash = _hash(key);
            if(HashNodeType* existing = _find_node(hash, key)) return existing;
            return _insert_with_hash(hash, key, val);
        }

        void _rehash(size_t new_capacity) {
            new_capacity = _next_power_of_two(std::max<size_t>(new_capacity, 1));
            BucketType new_buckets(new_capacity, nullptr);
            auto old_capacity = buckets.size();
            for(size_t i = 0; i < old_capacity; ++i) {
                HashNodeType* node = buckets[i];
                while(node != nullptr) {
                    HashNodeType* next = node->next_data();
                    auto target = node->hash_data() & (new_capacity - 1);
                    node->next_data() = new_buckets[target];
                    new_buckets[target] = node;
                    node = next;
                }
            }
            buckets.swap(new_buckets);
            _prepare_growth_metrics(buckets.size());
        }

        HashNodeType* _insert_with_hash(HashType hash, const K& key, const V& val) {
            if(_needs_rehash(sz + 1)) {
                auto new_capacity = buckets.empty() ? default_bucket_count : buckets.size() * 2;
                _rehash(new_capacity);
            } else if(buckets.empty()) {
                _rehash(default_bucket_count);
            }
            auto i = _bucket_index(hash);
            HashNodeType* new_node = HashNodeType::create_node(hash, key, val, buckets[i], __alloc);
            buckets[i] = new_node;
            ++sz;
            return new_node;
        }

        HashNodeType* _insert_impl(const K& key, const V& val) {
            return _insert_with_hash(_hash(key), key, val);
        }
        public:

        __hash_map() {
            _init_buckets(default_bucket_count);
        }

        explicit __hash_map(size_t init_size) {
            _init_buckets(init_size);
        }

        __hash_map(const __hash_map&) = delete;
        __hash_map& operator=(const __hash_map&) = delete;

        __hash_map(__hash_map&& other) noexcept : __alloc(other.__alloc), buckets(), sz(0), max_load_factor(other.max_load_factor), bucket_mask(other.bucket_mask), growth_threshold(other.growth_threshold) {
            buckets.swap(other.buckets);
            sz = other.sz;
            other.sz = 0;
            other.bucket_mask = 0;
            other.growth_threshold = 0;
        }

        __hash_map& operator=(__hash_map&& other) noexcept {
            if(this == &other) return *this;
            _destroy_nodes();
            buckets.swap(other.buckets);
            __alloc = other.__alloc;
            sz = other.sz;
            max_load_factor = other.max_load_factor;
            bucket_mask = other.bucket_mask;
            growth_threshold = other.growth_threshold;
            other.sz = 0;
            other.bucket_mask = 0;
            other.growth_threshold = 0;
            return *this;
        }

        ~__hash_map() {
            _destroy_nodes();
        }

        inline V& insert(const K& key, const V& val) {
            auto hash = _hash(key);
            if(auto as = _find_node(hash, key)) { as->val_data() = val; return as->val_data(); }
            return _insert_with_hash(hash, key, val)->val_data();
        }

        inline V& at(const K& key) { return _find_impl(key)->val_data(); }
        inline const V& at(const K& key) const { return _find_impl(key)->val_data(); }
        inline size_t size() const { return sz; }
        inline V& operator[](const K& key) { return _find_or_insert(key)->val_data(); }
        inline const V& operator[](const K& key) const { return _find_impl(key)->val_data(); }

        bool contains(const K& key) {
            return _find_node(_hash(key), key) != nullptr;
        }

        inline void erase(const K& key) {
            _erase_impl(key);
        }
        __hash_map_iterator<K, V> begin() {
            if(buckets.empty()) return end();
            for(size_t i = 0; i < buckets.size(); ++i) {
                auto* node = buckets[i];
                if(node != nullptr) return __hash_map_iterator<K, V>(node, buckets, i);
            }
            return end();
        }

        __hash_map_iterator<K, V> end() {
            return __hash_map_iterator<K, V>(nullptr, buckets, buckets.size());
        }
        
    };

    template<typename K, typename V, class Hasher=Hash<K>, class Alloc=Basic_Template_Allocator<__hash_map_node<K, V>>>
    using HashMap = __hash_map<K, V, Hasher, Alloc>;
}

#endif
