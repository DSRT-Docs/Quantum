#pragma once

/**
 * @file DSRTMap.h
 * @brief Hash map implementation
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include "DSRTMemory.h"
#include "DSRTHash.h"
#include <initializer_list>

namespace DSRT {
    
    /**
     * @brief Hash map entry
     * @tparam Key Key type
     * @tparam Value Value type
     */
    template<typename Key, typename Value>
    struct MapEntry {
        Key key;
        Value value;
        size_t hash;
        MapEntry* next;
        
        /**
         * @brief Constructs an empty entry
         */
        MapEntry() : next(nullptr) {}
        
        /**
         * @brief Constructs an entry with key and value
         * @param k Key
         * @param v Value
         * @param h Hash value
         */
        MapEntry(const Key& k, const Value& v, size_t h)
            : key(k), value(v), hash(h), next(nullptr) {}
        
        /**
         * @brief Constructs an entry with key and value (move)
         * @param k Key
         * @param v Value
         * @param h Hash value
         */
        MapEntry(Key&& k, Value&& v, size_t h)
            : key(std::move(k)), value(std::move(v)), hash(h), next(nullptr) {}
    };
    
    /**
     * @brief Hash map iterator
     * @tparam Key Key type
     * @tparam Value Value type
     * @tparam IsConst Whether iterator is const
     */
    template<typename Key, typename Value, bool IsConst>
    class MapIterator {
    public:
        using EntryType = MapEntry<Key, Value>;
        using EntryPointer = typename std::conditional<IsConst, 
                                                      const EntryType*, 
                                                      EntryType*>::type;
        using EntryReference = typename std::conditional<IsConst,
                                                        const EntryType&,
                                                        EntryType&>::type;
        
        using KeyType = Key;
        using ValueType = Value;
        using PairType = typename std::conditional<IsConst,
                                                  const std::pair<const Key, Value>,
                                                  std::pair<const Key, Value>>::type;
        
        /**
         * @brief Constructs an iterator
         * @param entry Current entry
         * @param bucket Bucket index
         * @param buckets Bucket array
         * @param bucketCount Number of buckets
         */
        MapIterator(EntryPointer entry, size_t bucket, 
                   EntryPointer* buckets, size_t bucketCount)
            : m_entry(entry), m_bucket(bucket), 
              m_buckets(buckets), m_bucketCount(bucketCount) {
            // Advance to first valid entry if needed
            if (!m_entry) {
                NextBucket();
            }
        }
        
        /**
         * @brief Dereference operator
         * @return Reference to key-value pair
         */
        PairType& operator*() const {
            return reinterpret_cast<PairType&>(m_entry->key);
        }
        
        /**
         * @brief Arrow operator
         * @return Pointer to key-value pair
         */
        PairType* operator->() const {
            return reinterpret_cast<PairType*>(&m_entry->key);
        }
        
        /**
         * @brief Pre-increment operator
         * @return Reference to this iterator
         */
        MapIterator& operator++() {
            Next();
            return *this;
        }
        
        /**
         * @brief Post-increment operator
         * @return Copy of iterator before increment
         */
        MapIterator operator++(int) {
            MapIterator temp = *this;
            Next();
            return temp;
        }
        
        /**
         * @brief Equality operator
         * @param other Iterator to compare
         * @return True if equal
         */
        bool operator==(const MapIterator& other) const {
            return m_entry == other.m_entry;
        }
        
        /**
         * @brief Inequality operator
         * @param other Iterator to compare
         * @return True if not equal
         */
        bool operator!=(const MapIterator& other) const {
            return !(*this == other);
        }
        
    private:
        void Next() {
            if (!m_entry) return;
            
            // Move to next entry in chain
            m_entry = m_entry->next;
            
            // If end of chain, move to next bucket
            if (!m_entry) {
                NextBucket();
            }
        }
        
        void NextBucket() {
            if (!m_buckets || m_bucket >= m_bucketCount) return;
            
            // Find next non-empty bucket
            while (++m_bucket < m_bucketCount) {
                m_entry = m_buckets[m_bucket];
                if (m_entry) break;
            }
            
            // If no more buckets, set to null
            if (m_bucket >= m_bucketCount) {
                m_entry = nullptr;
            }
        }
        
        EntryPointer m_entry;
        size_t m_bucket;
        EntryPointer* m_buckets;
        size_t m_bucketCount;
    };
    
    /**
     * @brief Hash map class
     * @tparam Key Key type
     * @tparam Value Value type
     * @tparam Hasher Hash function type
     */
    template<typename Key, typename Value, 
             typename Hasher = Hash<Key>>
    class Map {
    public:
        using KeyType = Key;
        using ValueType = Value;
        using PairType = std::pair<const Key, Value>;
        using EntryType = MapEntry<Key, Value>;
        using Iterator = MapIterator<Key, Value, false>;
        using ConstIterator = MapIterator<Key, Value, true>;
        using SizeType = size_t;
        
        static constexpr size_t DEFAULT_CAPACITY = 16;
        static constexpr float MAX_LOAD_FACTOR = 0.75f;
        
        /**
         * @brief Constructs an empty map
         */
        Map() : m_buckets(nullptr), m_bucketCount(0), 
                m_size(0), m_hasher() {
            Rehash(DEFAULT_CAPACITY);
        }
        
        /**
         * @brief Constructs a map with initial capacity
         * @param capacity Initial capacity
         */
        explicit Map(size_t capacity) : m_buckets(nullptr), 
                                        m_bucketCount(0), 
                                        m_size(0), 
                                        m_hasher() {
            Rehash(CalculateBucketCount(capacity));
        }
        
        /**
         * @brief Constructs a map from initializer list
         * @param list Initializer list
         */
        Map(std::initializer_list<PairType> list) 
            : m_buckets(nullptr), m_bucketCount(0), m_size(0), m_hasher() {
            Rehash(CalculateBucketCount(list.size()));
            for (const auto& pair : list) {
                Insert(pair.first, pair.second);
            }
        }
        
        /**
         * @brief Copy constructor
         * @param other Map to copy
         */
        Map(const Map& other) : m_buckets(nullptr), 
                                m_bucketCount(0), 
                                m_size(0), 
                                m_hasher() {
            Rehash(other.m_bucketCount);
            for (const auto& pair : other) {
                Insert(pair.first, pair.second);
            }
        }
        
        /**
         * @brief Move constructor
         * @param other Map to move
         */
        Map(Map&& other) noexcept 
            : m_buckets(other.m_buckets)
            , m_bucketCount(other.m_bucketCount)
            , m_size(other.m_size)
            , m_hasher(std::move(other.m_hasher)) {
            other.m_buckets = nullptr;
            other.m_bucketCount = 0;
            other.m_size = 0;
        }
        
        /**
         * @brief Destructor
         */
        ~Map() {
            Clear();
            Memory::Free(m_buckets);
        }
        
        /**
         * @brief Copy assignment operator
         * @param other Map to copy
         * @return Reference to this map
         */
        Map& operator=(const Map& other) {
            if (this != &other) {
                Clear();
                Rehash(other.m_bucketCount);
                for (const auto& pair : other) {
                    Insert(pair.first, pair.second);
                }
            }
            return *this;
        }
        
        /**
         * @brief Move assignment operator
         * @param other Map to move
         * @return Reference to this map
         */
        Map& operator=(Map&& other) noexcept {
            if (this != &other) {
                Clear();
                Memory::Free(m_buckets);
                
                m_buckets = other.m_buckets;
                m_bucketCount = other.m_bucketCount;
                m_size = other.m_size;
                m_hasher = std::move(other.m_hasher);
                
                other.m_buckets = nullptr;
                other.m_bucketCount = 0;
                other.m_size = 0;
            }
            return *this;
        }
        
        /**
         * @brief Array subscript operator
         * @param key Key to look up
         * @return Reference to value
         */
        Value& operator[](const Key& key) {
            return Insert(key, Value()).first->second;
        }
        
        /**
         * @brief Array subscript operator (move)
         * @param key Key to look up
         * @return Reference to value
         */
        Value& operator[](Key&& key) {
            return Insert(std::move(key), Value()).first->second;
        }
        
        // Capacity
        
        /**
         * @brief Gets map size
         * @return Number of elements
         */
        SizeType Size() const { return m_size; }
        
        /**
         * @brief Gets bucket count
         * @return Number of buckets
         */
        SizeType BucketCount() const { return m_bucketCount; }
        
        /**
         * @brief Gets current load factor
         * @return Load factor
         */
        float LoadFactor() const {
            return m_bucketCount > 0 ? static_cast<float>(m_size) / m_bucketCount : 0.0f;
        }
        
        /**
         * @brief Checks if map is empty
         * @return True if empty
         */
        bool IsEmpty() const { return m_size == 0; }
        
        // Element access
        
        /**
         * @brief Gets value for key
         * @param key Key to look up
         * @return Reference to value
         * @throws std::out_of_range if key not found
         */
        Value& At(const Key& key) {
            EntryType* entry = FindEntry(key);
            if (!entry) {
                // Throw or handle error
                DSRT_ASSERT_MSG(false, "Key not found in map");
            }
            return entry->value;
        }
        
        /**
         * @brief Gets value for key (const)
         * @param key Key to look up
         * @return Const reference to value
         * @throws std::out_of_range if key not found
         */
        const Value& At(const Key& key) const {
            EntryType* entry = FindEntry(key);
            if (!entry) {
                // Throw or handle error
                DSRT_ASSERT_MSG(false, "Key not found in map");
            }
            return entry->value;
        }
        
        // Modifiers
        
        /**
         * @brief Clears the map
         */
        void Clear() {
            for (size_t i = 0; i < m_bucketCount; ++i) {
                EntryType* entry = m_buckets[i];
                while (entry) {
                    EntryType* next = entry->next;
                    DestroyEntry(entry);
                    entry = next;
                }
                m_buckets[i] = nullptr;
            }
            m_size = 0;
        }
        
        /**
         * @brief Inserts key-value pair
         * @param key Key to insert
         * @param value Value to insert
         * @return Pair of iterator and bool indicating insertion
         */
        std::pair<Iterator, bool> Insert(const Key& key, const Value& value) {
            return InsertImpl(key, value);
        }
        
        /**
         * @brief Inserts key-value pair (move)
         * @param key Key to insert
         * @param value Value to insert
         * @return Pair of iterator and bool indicating insertion
         */
        std::pair<Iterator, bool> Insert(Key&& key, Value&& value) {
            return InsertImpl(std::move(key), std::move(value));
        }
        
        /**
         * @brief Inserts key-value pair if key doesn't exist
         * @param key Key to insert
         * @param value Value to insert
         * @return Pair of iterator and bool indicating insertion
         */
        std::pair<Iterator, bool> InsertOrAssign(const Key& key, const Value& value) {
            auto result = Insert(key, value);
            if (!result.second) {
                result.first->second = value;
            }
            return result;
        }
        
        /**
         * @brief Inserts key-value pair if key doesn't exist (move)
         * @param key Key to insert
         * @param value Value to insert
         * @return Pair of iterator and bool indicating insertion
         */
        std::pair<Iterator, bool> InsertOrAssign(Key&& key, Value&& value) {
            auto result = Insert(std::move(key), std::move(value));
            if (!result.second) {
                result.first->second = std::move(value);
            }
            return result;
        }
        
        /**
         * @brief Erases element by key
         * @param key Key to erase
         * @return Number of elements erased (0 or 1)
         */
        size_t Erase(const Key& key) {
            size_t hash = m_hasher(key);
            size_t bucket = hash % m_bucketCount;
            
            EntryType* prev = nullptr;
            EntryType* entry = m_buckets[bucket];
            
            while (entry) {
                if (entry->hash == hash && entry->key == key) {
                    if (prev) {
                        prev->next = entry->next;
                    } else {
                        m_buckets[bucket] = entry->next;
                    }
                    
                    DestroyEntry(entry);
                    --m_size;
                    return 1;
                }
                
                prev = entry;
                entry = entry->next;
            }
            
            return 0;
        }
        
        /**
         * @brief Erases element at position
         * @param position Iterator to element
         * @return Iterator to next element
         */
        Iterator Erase(ConstIterator position) {
            if (position == End()) return End();
            
            size_t hash = position.m_entry->hash;
            size_t bucket = hash % m_bucketCount;
            
            EntryType* prev = nullptr;
            EntryType* entry = m_buckets[bucket];
            
            while (entry && entry != position.m_entry) {
                prev = entry;
                entry = entry->next;
            }
            
            if (!entry) return End();
            
            EntryType* next = entry->next;
            if (prev) {
                prev->next = next;
            } else {
                m_buckets[bucket] = next;
            }
            
            DestroyEntry(entry);
            --m_size;
            
            return Iterator(next, bucket, m_buckets, m_bucketCount);
        }
        
        /**
         * @brief Reserves capacity
         * @param capacity Minimum capacity
         */
        void Reserve(size_t capacity) {
            size_t requiredBuckets = CalculateBucketCount(capacity);
            if (requiredBuckets > m_bucketCount) {
                Rehash(requiredBuckets);
            }
        }
        
        /**
         * @brief Rehashes the map
         * @param bucketCount New bucket count
         */
        void Rehash(size_t bucketCount) {
            if (bucketCount <= m_bucketCount && 
                LoadFactor() <= MAX_LOAD_FACTOR) {
                return;
            }
            
            bucketCount = CalculateBucketCount(bucketCount);
            if (bucketCount == m_bucketCount) return;
            
            // Allocate new buckets
            EntryType** newBuckets = static_cast<EntryType**>(
                Memory::Allocate(bucketCount * sizeof(EntryType*)));
            Memory::Zero(newBuckets, bucketCount * sizeof(EntryType*));
            
            // Rehash all entries
            for (size_t i = 0; i < m_bucketCount; ++i) {
                EntryType* entry = m_buckets[i];
                while (entry) {
                    EntryType* next = entry->next;
                    
                    size_t newBucket = entry->hash % bucketCount;
                    entry->next = newBuckets[newBucket];
                    newBuckets[newBucket] = entry;
                    
                    entry = next;
                }
            }
            
            // Replace old buckets
            Memory::Free(m_buckets);
            m_buckets = newBuckets;
            m_bucketCount = bucketCount;
        }
        
        // Lookup
        
        /**
         * @brief Finds element by key
         * @param key Key to find
         * @return Iterator to element, or End() if not found
         */
        Iterator Find(const Key& key) {
            EntryType* entry = FindEntry(key);
            if (!entry) return End();
            
            size_t bucket = entry->hash % m_bucketCount;
            return Iterator(entry, bucket, m_buckets, m_bucketCount);
        }
        
        /**
         * @brief Finds element by key (const)
         * @param key Key to find
         * @return Const iterator to element, or End() if not found
         */
        ConstIterator Find(const Key& key) const {
            EntryType* entry = FindEntry(key);
            if (!entry) return End();
            
            size_t bucket = entry->hash % m_bucketCount;
            return ConstIterator(entry, bucket, m_buckets, m_bucketCount);
        }
        
        /**
         * @brief Checks if map contains key
         * @param key Key to check
         * @return True if contains key
         */
        bool Contains(const Key& key) const {
            return FindEntry(key) != nullptr;
        }
        
        /**
         * @brief Counts elements with key (0 or 1)
         * @param key Key to count
         * @return Number of elements with key
         */
        size_t Count(const Key& key) const {
            return FindEntry(key) ? 1 : 0;
        }
        
        // Iterators
        
        /**
         * @brief Gets iterator to beginning
         * @return Iterator to first element
         */
        Iterator Begin() {
            return Iterator(FindFirstEntry(), 0, m_buckets, m_bucketCount);
        }
        
        /**
         * @brief Gets iterator to beginning (const)
         * @return Const iterator to first element
         */
        ConstIterator Begin() const {
            return ConstIterator(FindFirstEntry(), 0, m_buckets, m_bucketCount);
        }
        
        /**
         * @brief Gets iterator to end
         * @return Iterator to one past last element
         */
        Iterator End() {
            return Iterator(nullptr, m_bucketCount, m_buckets, m_bucketCount);
        }
        
        /**
         * @brief Gets iterator to end (const)
         * @return Const iterator to one past last element
         */
        ConstIterator End() const {
            return ConstIterator(nullptr, m_bucketCount, m_buckets, m_bucketCount);
        }
        
    private:
        EntryType** m_buckets;
        size_t m_bucketCount;
        size_t m_size;
        Hasher m_hasher;
        
        /**
         * @brief Calculates bucket count for given capacity
         * @param capacity Requested capacity
         * @return Bucket count
         */
        static size_t CalculateBucketCount(size_t capacity) {
            // Find next power of two
            size_t count = 1;
            while (count < capacity) {
                count <<= 1;
            }
            return count;
        }
        
        /**
         * @brief Finds entry by key
         * @param key Key to find
         * @return Pointer to entry, or nullptr if not found
         */
        EntryType* FindEntry(const Key& key) const {
            if (!m_buckets) return nullptr;
            
            size_t hash = m_hasher(key);
            size_t bucket = hash % m_bucketCount;
            
            EntryType* entry = m_buckets[bucket];
            while (entry) {
                if (entry->hash == hash && entry->key == key) {
                    return entry;
                }
                entry = entry->next;
            }
            
            return nullptr;
        }
        
        /**
         * @brief Finds first non-empty bucket
         * @return Pointer to first entry
         */
        EntryType* FindFirstEntry() const {
            if (!m_buckets) return nullptr;
            
            for (size_t i = 0; i < m_bucketCount; ++i) {
                if (m_buckets[i]) {
                    return m_buckets[i];
                }
            }
            
            return nullptr;
        }
        
        /**
         * @brief Creates a new entry
         * @param key Key
         * @param value Value
         * @param hash Hash value
         * @return Pointer to new entry
         */
        EntryType* CreateEntry(const Key& key, const Value& value, size_t hash) {
            void* memory = Memory::Allocate(sizeof(EntryType));
            return new (memory) EntryType(key, value, hash);
        }
        
        /**
         * @brief Creates a new entry (move)
         * @param key Key
         * @param value Value
         * @param hash Hash value
         * @return Pointer to new entry
         */
        EntryType* CreateEntry(Key&& key, Value&& value, size_t hash) {
            void* memory = Memory::Allocate(sizeof(EntryType));
            return new (memory) EntryType(std::move(key), std::move(value), hash);
        }
        
        /**
         * @brief Destroys an entry
         * @param entry Entry to destroy
         */
        void DestroyEntry(EntryType* entry) {
            entry->~EntryType();
            Memory::Free(entry);
        }
        
        /**
         * @brief Implementation of insert
         * @tparam K Key type
         * @tparam V Value type
         * @param key Key to insert
         * @param value Value to insert
         * @return Pair of iterator and bool
         */
        template<typename K, typename V>
        std::pair<Iterator, bool> InsertImpl(K&& key, V&& value) {
            // Check if rehash is needed
            if (m_size + 1 > m_bucketCount * MAX_LOAD_FACTOR) {
                Rehash(m_bucketCount * 2);
            }
            
            size_t hash = m_hasher(key);
            size_t bucket = hash % m_bucketCount;
            
            // Check for existing key
            EntryType* entry = m_buckets[bucket];
            while (entry) {
                if (entry->hash == hash && entry->key == key) {
                    return std::make_pair(
                        Iterator(entry, bucket, m_buckets, m_bucketCount),
                        false
                    );
                }
                entry = entry->next;
            }
            
            // Create new entry
            EntryType* newEntry = CreateEntry(
                std::forward<K>(key), 
                std::forward<V>(value), 
                hash
            );
            
            // Insert at beginning of chain
            newEntry->next = m_buckets[bucket];
            m_buckets[bucket] = newEntry;
            ++m_size;
            
            return std::make_pair(
                Iterator(newEntry, bucket, m_buckets, m_bucketCount),
                true
            );
        }
    };
    
} // namespace DSRT
