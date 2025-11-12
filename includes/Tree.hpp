#ifndef __VELO_TREE
#define __VELO_TREE

#include "Allocator.hpp"
#include "Hash.hpp"
#include "Vector.hpp"
#include <cstddef>
#include <cassert>
namespace VELO {


    template<typename T, typename HashType=size_t>
    class __rb_tree_node {
        public:
        enum class __rb_tree_colors : bool {
            BLACK, RED
        };
        private:
        using NodeColorType = __rb_tree_colors;
        HashType hash;
        T key;
        NodeColorType color;

        class __rb_tree_node* left{nullptr};
        class __rb_tree_node* right{nullptr};
        class __rb_tree_node* parent{nullptr};
        size_t sz; // Subtree count
        
        public:

        explicit __rb_tree_node(HashType h, const T& k, NodeColorType c, size_t size) : hash(h), key(k), color(c), sz(size) {}
        
        inline HashType& hash_data() { return hash; }
        inline const HashType& hash_data() const { return hash; }
        
        inline T& key_data() { return key; }
        inline const T& key_data() const { return key; }

        inline NodeColorType& color_data() { return color; }
        inline const NodeColorType& color_data() const { return color; }

        inline __rb_tree_node*& left_data() { return left; }
        inline __rb_tree_node*& right_data() { return right; }
        inline __rb_tree_node*& parent_data() { return parent; }
        inline const __rb_tree_node* left_data() const { return left; }
        inline const __rb_tree_node* right_data() const { return right; }
        inline const __rb_tree_node* parent_data() const { return parent; }
        
        inline size_t& size_data() { return sz; }
        inline const size_t& size_data() const { return sz; }
        inline bool is_red() const { return color == NodeColorType::RED; }

        inline bool operator<(const __rb_tree_node& node) { return hash < node.hash; }
        inline bool operator>(const __rb_tree_node& node) { return hash > node.hash; }

    };

    template<typename T, class Hasher = VELO::Hash<T>, class Alloc = Basic_Template_Allocator<__rb_tree_node<T>>>
    class __rb_tree {
        using NodeType = __rb_tree_node<T>;
        using NodeColorType = typename NodeType::__rb_tree_colors;
        using HashType = size_t;
        Alloc __alloc;
        NodeType* root{nullptr};

        inline static bool _is_red(NodeType* node) { return (node==nullptr) ? false : (node->is_red()); }
        inline static size_t _size(NodeType* node) { return (node==nullptr) ? 0 : node->size_data(); }

        inline static HashType _hash(const T& key) { return Hasher()(key); }

        NodeType* _create_node(HashType hash, const T& key) {
            NodeType* node = __alloc.alloc(1);
            new (node) NodeType(hash, key, NodeColorType::RED, 1);
            node->left_data() = nullptr;
            node->right_data() = nullptr;
            node->parent_data() = nullptr;
            return node;
        }

        inline void _recalc_size(NodeType* node) {
            if(node == nullptr) return;
            node->size_data() = _size(node->left_data()) + _size(node->right_data()) + 1;
        }

        void _update_size_up(NodeType* node) {
            while(node != nullptr) {
                _recalc_size(node);
                node = node->parent_data();
            }
        }

        NodeType* _rotate_left(NodeType* x) {
            NodeType* y = x->right_data();
            assert(y != nullptr);
            x->right_data() = y->left_data();
            if(y->left_data() != nullptr) y->left_data()->parent_data() = x;
            y->parent_data() = x->parent_data();
            if(x->parent_data() == nullptr) {
                root = y;
            } else if(x == x->parent_data()->left_data()) {
                x->parent_data()->left_data() = y;
            } else {
                x->parent_data()->right_data() = y;
            }
            y->left_data() = x;
            x->parent_data() = y;
            _recalc_size(x);
            _recalc_size(y);
            if(y->parent_data() != nullptr) _recalc_size(y->parent_data());
            return y;
        }

        NodeType* _rotate_right(NodeType* y) {
            NodeType* x = y->left_data();
            assert(x != nullptr);
            y->left_data() = x->right_data();
            if(x->right_data() != nullptr) x->right_data()->parent_data() = y;
            x->parent_data() = y->parent_data();
            if(y->parent_data() == nullptr) {
                root = x;
            } else if(y == y->parent_data()->left_data()) {
                y->parent_data()->left_data() = x;
            } else {
                y->parent_data()->right_data() = x;
            }
            x->right_data() = y;
            y->parent_data() = x;
            _recalc_size(y);
            _recalc_size(x);
            if(x->parent_data() != nullptr) _recalc_size(x->parent_data());
            return x;
        }

        void _insert_fixup(NodeType* node) {
            while(node != root && _is_red(node->parent_data())) {
                NodeType* parent = node->parent_data();
                NodeType* grand = parent->parent_data();
                if(grand == nullptr) break;
                if(parent == grand->left_data()) {
                    NodeType* uncle = grand->right_data();
                    if(_is_red(uncle)) {
                        parent->color_data() = NodeColorType::BLACK;
                        uncle->color_data() = NodeColorType::BLACK;
                        grand->color_data() = NodeColorType::RED;
                        node = grand;
                    } else {
                        if(node == parent->right_data()) {
                            node = parent;
                            _rotate_left(node);
                            parent = node->parent_data();
                            grand = parent ? parent->parent_data() : nullptr;
                        }
                        if(parent != nullptr) parent->color_data() = NodeColorType::BLACK;
                        if(grand != nullptr) {
                            grand->color_data() = NodeColorType::RED;
                            _rotate_right(grand);
                        }
                    }
                } else {
                    NodeType* uncle = grand->left_data();
                    if(_is_red(uncle)) {
                        parent->color_data() = NodeColorType::BLACK;
                        uncle->color_data() = NodeColorType::BLACK;
                        grand->color_data() = NodeColorType::RED;
                        node = grand;
                    } else {
                        if(node == parent->left_data()) {
                            node = parent;
                            _rotate_right(node);
                            parent = node->parent_data();
                            grand = parent ? parent->parent_data() : nullptr;
                        }
                        if(parent != nullptr) parent->color_data() = NodeColorType::BLACK;
                        if(grand != nullptr) {
                            grand->color_data() = NodeColorType::RED;
                            _rotate_left(grand);
                        }
                    }
                }
            }
            if(root != nullptr) root->color_data() = NodeColorType::BLACK;
        }

        void _transplant(NodeType* u, NodeType* v) {
            if(u->parent_data() == nullptr) root = v;
            else if(u == u->parent_data()->left_data()) u->parent_data()->left_data() = v;
            else u->parent_data()->right_data() = v;
            if(v != nullptr) v->parent_data() = u->parent_data();
        }

        void _erase_fixup(NodeType* x, NodeType* parent) {
            while(x != root && !_is_red(x)) {
                if(parent == nullptr) break;
                if(x == parent->left_data()) {
                    NodeType* w = parent->right_data();
                    if(_is_red(w)) {
                        w->color_data() = NodeColorType::BLACK;
                        parent->color_data() = NodeColorType::RED;
                        _rotate_left(parent);
                        w = parent->right_data();
                    }
                    if(!_is_red(w ? w->left_data() : nullptr) && !_is_red(w ? w->right_data() : nullptr)) {
                        if(w) w->color_data() = NodeColorType::RED;
                        x = parent;
                        parent = x ? x->parent_data() : nullptr;
                    } else {
                        if(!_is_red(w ? w->right_data() : nullptr)) {
                            if(w && w->left_data()) w->left_data()->color_data() = NodeColorType::BLACK;
                            if(w) {
                                w->color_data() = NodeColorType::RED;
                                _rotate_right(w);
                            }
                            w = parent->right_data();
                        }
                        if(w) w->color_data() = parent->color_data();
                        parent->color_data() = NodeColorType::BLACK;
                        if(w && w->right_data()) w->right_data()->color_data() = NodeColorType::BLACK;
                        _rotate_left(parent);
                        x = root;
                        parent = nullptr;
                    }
                } else {
                    NodeType* w = parent->left_data();
                    if(_is_red(w)) {
                        w->color_data() = NodeColorType::BLACK;
                        parent->color_data() = NodeColorType::RED;
                        _rotate_right(parent);
                        w = parent->left_data();
                    }
                    if(!_is_red(w ? w->right_data() : nullptr) && !_is_red(w ? w->left_data() : nullptr)) {
                        if(w) w->color_data() = NodeColorType::RED;
                        x = parent;
                        parent = x ? x->parent_data() : nullptr;
                    } else {
                        if(!_is_red(w ? w->left_data() : nullptr)) {
                            if(w && w->right_data()) w->right_data()->color_data() = NodeColorType::BLACK;
                            if(w) {
                                w->color_data() = NodeColorType::RED;
                                _rotate_left(w);
                            }
                            w = parent->left_data();
                        }
                        if(w) w->color_data() = parent->color_data();
                        parent->color_data() = NodeColorType::BLACK;
                        if(w && w->left_data()) w->left_data()->color_data() = NodeColorType::BLACK;
                        _rotate_right(parent);
                        x = root;
                        parent = nullptr;
                    }
                }
            }
            if(x) x->color_data() = NodeColorType::BLACK;
        }

        static NodeType* _min_node(NodeType* node) {
            if(node == nullptr) return nullptr;
            while(node->left_data() != nullptr) node = node->left_data();
            return node;
        }

        NodeType* _find_node(HashType hash, const T& key) {
            NodeType* cur = root;
            while(cur != nullptr) {
                if(hash < cur->hash_data()) cur = cur->left_data();
                else if(hash > cur->hash_data()) cur = cur->right_data();
                else {
                    return cur;
                }
            }
            return nullptr;
        }

        const NodeType* _find_node(HashType hash, const T& key) const {
            const NodeType* cur = root;
            while(cur != nullptr) {
                if(hash < cur->hash_data()) cur = cur->left_data();
                else if(hash > cur->hash_data()) cur = cur->right_data();
                else {
                    return cur;
                }
            }
            return nullptr;
        }

        void _clear(NodeType* node) {
            if(node == nullptr) return;
            _clear(node->left_data());
            _clear(node->right_data());
            node->~NodeType();
            __alloc.dealloc(node);
        }

        template<typename T1>
        class __rb_tree_iterator : Iterator<T1> {
            NodeType* cur{nullptr};
            public:
                __rb_tree_iterator() = default;
                explicit __rb_tree_iterator(NodeType* c) : cur(c) {}

                inline T1& operator*() const { return cur->key_data(); }
                inline __rb_tree_iterator& operator++() {
                    if(cur == nullptr) return *this;
                    if(cur->right_data() != nullptr) {
                        cur = cur->right_data();
                        while(cur->left_data() != nullptr) cur = cur->left_data();
                        return *this;
                    }
                    NodeType* parent = cur->parent_data();
                    while(parent != nullptr && cur == parent->right_data()) {
                        cur = parent;
                        parent = parent->parent_data();
                    }
                    cur = parent;
                    return *this;
                }

                inline bool operator==(const __rb_tree_iterator& iter) const { return cur == iter.cur; }
                inline bool operator!=(const __rb_tree_iterator& iter) const { return !(cur == iter.cur); }
        };

        public:

        using IterType = __rb_tree_iterator<T>;
        using ConstIterType = __rb_tree_iterator<const T>;


        __rb_tree() = default;
        explicit __rb_tree(const Alloc& alloc) : __alloc(alloc) {}
        __rb_tree(const __rb_tree&) = delete;
        __rb_tree& operator=(const __rb_tree&) = delete;
        __rb_tree(__rb_tree&&) = delete;
        __rb_tree& operator=(__rb_tree&&) = delete;
        ~__rb_tree() { clear(); }

        inline bool empty() { return root == nullptr; }
        inline size_t size() const { return _size(root); }
        void clear() {
            _clear(root);
            root = nullptr;
        }
        
        void insert(const T& key) {
            auto hash = _hash(key);
            NodeType* parent = nullptr;
            NodeType* cur = root;
            while(cur != nullptr) {
                parent = cur;
                if(hash < cur->hash_data()) cur = cur->left_data();
                else if(hash > cur->hash_data()) cur = cur->right_data();
                else {
                    cur->key_data() = key;
                    return;
                }
            }

            NodeType* node = _create_node(hash, key);
            node->parent_data() = parent;
            if(parent == nullptr) root = node;
            else if(hash < parent->hash_data()) parent->left_data() = node;
            else parent->right_data() = node;

            _insert_fixup(node);
            _update_size_up(node);
        }

        IterType find(const T& key) {
            return iterator(_find_node(_hash(key), key));
        }

        ConstIterType find(const T& key) const {
            return const_iterator(const_cast<NodeType*>(_find_node(_hash(key), key)));
        }

        bool erase(const T& key) {
            auto hash = _hash(key);
            NodeType* z = _find_node(hash, key);
            if(z == nullptr) return false;

            NodeType* y = z;
            NodeColorType y_original_color = y->color_data();
            NodeType* x = nullptr;
            NodeType* x_parent = nullptr;

            if(z->left_data() == nullptr) {
                x = z->right_data();
                x_parent = z->parent_data();
                _transplant(z, z->right_data());
            } else if(z->right_data() == nullptr) {
                x = z->left_data();
                x_parent = z->parent_data();
                _transplant(z, z->left_data());
            } else {
                y = _min_node(z->right_data());
                y_original_color = y->color_data();
                x = y->right_data();
                if(y->parent_data() == z) {
                    x_parent = y;
                } else {
                    _transplant(y, y->right_data());
                    y->right_data() = z->right_data();
                    if(y->right_data() != nullptr) y->right_data()->parent_data() = y;
                    x_parent = y->parent_data();
                }
                _transplant(z, y);
                y->left_data() = z->left_data();
                if(y->left_data() != nullptr) y->left_data()->parent_data() = y;
                y->color_data() = z->color_data();
                _recalc_size(y);
                x_parent = y;
            }

            if(y_original_color == NodeColorType::BLACK) {
                _erase_fixup(x, x_parent);
            }
            _update_size_up(x_parent ? x_parent : root);

            z->left_data() = nullptr;
            z->right_data() = nullptr;
            z->parent_data() = nullptr;
            z->~NodeType();
            __alloc.dealloc(z);
            return true;
        }
        
        inline IterType begin() { return IterType(_min_node(root)); }
        inline IterType end() { return IterType(nullptr); }
        inline ConstIterType begin() const { return ConstIterType(_min_node(root)); }
        inline ConstIterType end() const { return ConstIterType(nullptr); }
        inline ConstIterType cbegin() const { return ConstIterType(_min_node(root)); }
        inline ConstIterType cend() const { return ConstIterType(nullptr); }
    };

    template<typename T, class Hasher=Hash<T>, class Alloc=Basic_Template_Allocator<__rb_tree_node<T>>>
    using RBSet = __rb_tree<T, Hasher, Alloc>;
}

#endif 