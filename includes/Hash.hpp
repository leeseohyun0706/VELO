#ifndef __VELO_HASH
#define __VELO_HASH
#include <cstddef>
#include <cstdint>
#include <memory>
#include <array>
#include <string>
#include <type_traits>
namespace VELO {

    using u8 = uint8_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    static inline u32 rotl32(u32 x, int r) { return (x << r) | (x >> (32 - r)); }
    static inline u64 rotl64(u64 x, int r) { return (x << r) | (x >> (64 - r)); }
    static inline u32 rd32(const void* p) { u32 v; std::memcpy(&v, p, 4); return v; }
    static inline u64 rd64(const void* p) { u64 v; std::memcpy(&v, p, 8); return v; }


    static inline u32 fmix32(u32 h) {
        h ^= h >> 16; h *= 0x85ebca6bU;
        h ^= h >> 13; h *= 0xc2b2ae35U;
        h ^= h >> 16;
        return h;
    }
    static inline u64 fmix64(u64 k) {
        k ^= k >> 33; k *= 0xff51afd7ed558ccdULL;
        k ^= k >> 33; k *= 0xc4ceb9fe1a85ec53ULL;
        k ^= k >> 33;
        return k;
    }

    template<int Bits>
    class Murmur3HasherBase;
    
    template<>
    class Murmur3HasherBase<32> {
        private:
        static constexpr u32 C1 = 0xcc9e2d51U;
        static constexpr u32 C2 = 0x1b873593U;

        u32 seed{};
        u32 h1{};
        size_t total{};
        u8 buf[4];
        size_t bufsize{};

        inline void mixBlock(u32 k1) {
            k1*= C1; k1 = rotl32(k1, 15); k1 *= C2;
            h1 ^= k1;
            h1 = rotl32(h1, 13);
            h1 = h1 * 5U + 0xe6546b64U;
        }
        public:
        using result_type = u32;

        explicit Murmur3HasherBase(u32 seed = 0) { reset(seed); }

        void reset(u32 seed_ = 0) {
            seed = seed_;
            h1 = seed;
            total = 0;
            bufsize = 0;
        }

        void update(const void* data, size_t len) {
            const u8* p = static_cast<const u8*>(data);
            total+=len;

            if(bufsize + len < 4) {
                std::memcpy(buf + bufsize, p, len);
                bufsize += len;
                return;
            }

            size_t i = 0;
            if(bufsize > 0) {
                size_t fill = 4 - bufsize;
                std::memcpy(buf + bufsize, p, fill);
                mixBlock(rd32(buf));
                bufsize = 0;
                i+=fill;
            }

            for(; i+4 <= len; i+= 4) {
                u32 k1 = rd32(p+i);
                mixBlock(k1);
            }

            if(i < len) { bufsize = len - i; std::memcpy(buf, p+i, bufsize);}
        }
        

        result_type digest() const {
            u32 h = h1;
            u32 k1 = 0;

            switch (bufsize) {
                case 3: k1 ^= static_cast<u32>(buf[2]) << 16; [[fallthrough]];
                case 2: k1 ^= static_cast<u32>(buf[1]) << 8;  [[fallthrough]];
                case 1: k1 ^= static_cast<u32>(buf[0]);
                        k1 *= C1; k1 = rotl32(k1, 15); k1 *= C2; h ^= k1;
            }

            h ^= static_cast<u32>(total);

            h = fmix32(h);
            return h;
        }

        static result_type hash(const void* data, size_t len, u32 seed = 0) {
            Murmur3HasherBase<32> m(seed);
            m.update(data, len);
            return m.digest();
        }
    };

        template<>
    class Murmur3HasherBase<128> {
        private:
        static constexpr u64 C1 = 0x87c37b91114253d5ULL;
        static constexpr u64 C2 = 0x4cf5ad432745937fULL;

        u64 seed{};
        u64 h1{}, h2{};
        size_t total{};
        u8 buf[16];
        size_t bufsize{};

        inline void mixBlock(u64 k1, u64 k2) {
            // k1 round
            k1 *= C1; k1 = rotl64(k1, 31); k1 *= C2; h1 ^= k1;
            h1 = rotl64(h1, 27); h1 += h2; h1 = h1 * 5 + 0x52dce729;

            // k2 round
            k2 *= C2; k2 = rotl64(k2, 33); k2 *= C1; h2 ^= k2;
            h2 = rotl64(h2, 31); h2 += h1; h2 = h2 * 5 + 0x38495ab5;
        }
        public:
        using result_type = std::array<u64, 2>;

        explicit Murmur3HasherBase(u32 seed = 0) { reset(seed); }

        void reset(u32 seed_ = 0) {
            seed = seed_;
            h1 = seed;
            h2 = seed;
            total = 0;
            bufsize = 0;
        }

        void update(const void* data, size_t len) {
            const u8* p = static_cast<const u8*>(data);
            total+=len;

            if(bufsize + len < 16) {
                std::memcpy(buf + bufsize, p, len);
                bufsize += len;
                return;
            }

            size_t i = 0;
            if(bufsize > 0) {
                size_t fill = 16 - bufsize;
                std::memcpy(buf + bufsize, p, fill);
                mixBlock(rd64(buf+0), rd64(buf+8));
                bufsize = 0;
                i+=fill;
            }

            for(; i+16 <= len; i+= 16) {
                u64 k1 = rd64(p+i);
                u64 k2 = rd64(p+i+8);
                mixBlock(k1, k2);
            }

            if(i < len) { bufsize = len - i; std::memcpy(buf, p+i, bufsize);}
        }
        

        result_type digest() const {
            u64 h11 = h1;
            u64 h22 = h2;
            u64 k1 = 0, k2=0;

            switch (bufsize) {
                case 15: k2 ^= (u64)buf[14] << 48; [[fallthrough]];
                case 14: k2 ^= (u64)buf[13] << 40; [[fallthrough]];
                case 13: k2 ^= (u64)buf[12] << 32; [[fallthrough]];
                case 12: k2 ^= (u64)buf[11] << 24; [[fallthrough]];
                case 11: k2 ^= (u64)buf[10] << 16; [[fallthrough]];
                case 10: k2 ^= (u64)buf[9]  << 8;  [[fallthrough]];
                case  9: k2 ^= (u64)buf[8]  << 0;
                        k2 *= C2; k2 = rotl64(k2, 33); k2 *= C1; h22 ^= k2;
                        [[fallthrough]];
                case  8: k1 ^= (u64)buf[7]  << 56; [[fallthrough]];
                case  7: k1 ^= (u64)buf[6]  << 48; [[fallthrough]];
                case  6: k1 ^= (u64)buf[5]  << 40; [[fallthrough]];
                case  5: k1 ^= (u64)buf[4]  << 32; [[fallthrough]];
                case  4: k1 ^= (u64)buf[3]  << 24; [[fallthrough]];
                case  3: k1 ^= (u64)buf[2]  << 16; [[fallthrough]];
                case  2: k1 ^= (u64)buf[1]  << 8;  [[fallthrough]];
                case  1: k1 ^= (u64)buf[0]  << 0;
                        k1 *= C1; k1 = rotl64(k1, 31); k1 *= C2; h11 ^= k1;
            }

            h11 ^= (u64)total;
            h22 ^= (u64)total;

            // Finalization
            h11 += h22; h22 += h11;
            h11 = fmix64(h11);
            h22 = fmix64(h22);
            h11 += h22; h22 += h11;

            return {h11, h22};
        }

        static result_type hash(const void* data, size_t len, u32 seed = 0) {
            Murmur3HasherBase<128> m(seed);
            m.update(data, len);
            return m.digest();
        }
    };

    template<int Bits>
    class Murmur3Hasher {
        public:
        size_t operator()(const void* data, size_t len) const {
            if constexpr (Bits==32) {
                return static_cast<size_t>(Murmur3HasherBase<32>::hash(data, len));
            }else {
                auto h = Murmur3HasherBase<128>::hash(data, len);
                #if INTPTR_MAX == INT64_MAX
                            return static_cast<std::size_t>(h[0] ^ h[1]);
                #else
                            return static_cast<std::size_t>(h[0] ^ (h[1] >> 32));
                #endif
            }
        }
    };

    class SimpleStringHasher {
        public:
        size_t operator()(const char* str) const {
            unsigned int hash = 5381;

            while (*str != '\0')
            {
                hash = hash * 33 + *str;
                ++str;
            }
            return hash;
        }
    };

    template<typename Ty, class Hasher=Murmur3Hasher<sizeof(size_t)>>
    struct Hash;
    
    template<>
    struct Hash<int> {
        using KeyType = int;
        using HashType = size_t;
        size_t operator()(const int& s) const noexcept { return s; }
    };

    template<>
    struct Hash<size_t> {
        using KeyType = size_t;
        using HashType = size_t;
        size_t operator()(const size_t& s) const noexcept { return s; }
    };

    template<typename Ty>
    struct Hash<Ty*> {
        using KeyType = Ty*;
        using HashType = size_t;
        size_t operator()(const Ty* s, size_t len) const noexcept {
            return static_cast<size_t>(Murmur3Hasher<128>()(s, len));
        }
    };

    template<>
    struct Hash<std::string> {
        using KeyType = std::string;
        using HashType = size_t;
        size_t operator()(const std::string& s) const { return SimpleStringHasher()(s.c_str()); }
    };
}

#endif