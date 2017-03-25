/**
 C++ wrapper for ok_lib's hashmap.

 If ok_lib has not been defined elsewhere, include "ok_map.h": like this:

     #define OK_LIB_DEFINE
     #include "ok_map.h"
 */

#ifndef OK_MAP_H
#define OK_MAP_H

#define OK_LIB_DECLARE
#include "ok_lib.h"
#include <string>

namespace ok {
    template <typename T> static ok_hash_t hash_func(const T key);
    template <typename T> static bool equals_func(const void* v1, const void* v2);

    template <typename key_type, typename value_type>
    class map {
    public:
        explicit map(size_t initial_capacity = 0,
                     ok_hash_t (*key_hash_func)(key_type) = hash_func<key_type>,
                     bool (*key_equals_func)(const void*, const void*) = equals_func<key_type>) {
            ok_map_init_custom_with_capacity(&map_, key_hash_func, key_equals_func,
                                            initial_capacity);
        }

        // Copying disabled for now. Need a ok_map_init_from_map(from_map);
        map(const map& map) = delete;
        map& operator=(const map& map) = delete;

        virtual ~map() {
            ok_map_deinit(&map_);
        }

        inline size_t size() const {
            return ok_map_count(&map_);
        }

        inline bool put(const key_type& key, const value_type& value) {
            return ok_map_put(&map_, key, value);
        }

        inline value_type& get(const key_type& key) const {
            return ok_map_get(&map_, key);
        }

        inline value_type* get_ptr(const key_type& key) const {
            return ok_map_get_ptr(&map_, key);
        }

        /**
         Gets a reference to the value for the specified key.
         If a mapping to the key doesn't exist, it is created.
         */
        inline value_type& operator[](const key_type& key) {
            return *ok_map_put_and_get_ptr(&map_, key);
        }

        inline bool erase(const key_type& key) {
            return ok_map_remove(&map_, key);
        }

    private:
        class iterator;

    public:
        iterator begin() const {
            iterator i(&map_);
            i++;
            return i;
        }

        iterator end() const {
            return iterator(&map_);
        }

        struct pair {
            key_type key;
            value_type value;
        };

    private:
        typedef struct ok_map_of(key_type, value_type) map_t;
        map_t map_;

        class iterator {
        public:
            iterator(const map_t* map, void*iter = NULL) : map_(map), iter_(iter) { }
            iterator& operator++() {
                iter_ = _ok_map_next(map_->m, iter_, &pair_.key, sizeof(pair_.key),
                                     &pair_.value, sizeof(pair_.value));
                return *this;
            }
            iterator operator++(int) {
                iterator result(map_, iter_);
                ++(*this);
                return result;
            }
            pair& operator*() { return pair_; }
            pair* operator->() { return &pair_; }
            bool operator==(const iterator& rhs) { return iter_ == rhs.iter_; }
            bool operator!=(const iterator& rhs) { return iter_ != rhs.iter_; }
        private:
            const map_t* const map_;
            void* iter_;
            pair pair_;
        };
    };

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

    // MARK: Hash

    template <> ok_hash_t hash_func(int8_t key) { return ok_int8_hash(key); }

    template <> ok_hash_t hash_func(uint8_t key) { return ok_uint8_hash(key); }

    template <> ok_hash_t hash_func(int16_t key) { return ok_int16_hash(key); }

    template <> ok_hash_t hash_func(uint16_t key) { return ok_uint16_hash(key); }
    
    template <> ok_hash_t hash_func(int32_t key) { return ok_int32_hash(key); }

    template <> ok_hash_t hash_func(uint32_t key) { return ok_uint32_hash(key); }

    template <> ok_hash_t hash_func(int64_t key) { return ok_int64_hash(key); }

    template <> ok_hash_t hash_func(uint64_t key) { return ok_uint64_hash(key); }

    template <> ok_hash_t hash_func(float key) { return ok_float_hash(key); }

    template <> ok_hash_t hash_func(double key) { return ok_double_hash(key); }

    template <> ok_hash_t hash_func(const char* key) { return ok_const_str_hash(key); }

    template <> ok_hash_t hash_func(const std::string key) { return ok_const_str_hash(key.c_str()); }

    // MARK: Equals

    template <typename T> bool equals_func(const void* v1, const void* v2) {
        return *(T const*)v1 == *(T const*)v2;
    }

    template<> bool equals_func<const char *>(const void* v1, const void* v2) {
        const char* str1 = *(const char* const*)v1;
        const char* str2 = *(const char* const*)v2;

        return strcmp(str1, str2) == 0;
    }

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

}

#endif
