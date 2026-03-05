#ifndef FIXED_SIZE_MAP_HPP
#define FIXED_SIZE_MAP_HPP

#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace KTN {
namespace Core {

template <std::size_t Capacity>
class FixedSizeMap {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");

    struct Entry {
        char orderid[20];
        uint8_t callback_id;
        bool used;
    };

    Entry table[Capacity];
    std::size_t count;

    // FNV-1a hash function for 20-byte keys.
    static std::size_t hash(const char* key) {
        std::size_t h = 1469598103934665603ull;
        for (std::size_t i = 0; i < 20; ++i) {
            h ^= static_cast<std::size_t>(key[i]);
            h *= 1099511628211ull;
        }
        return h;
    }

public:
    FixedSizeMap() : count(0) {
        for (std::size_t i = 0; i < Capacity; ++i)
            table[i].used = false;
    }

    // Insert or update the key-value mapping.
    void insert(const char key[20], uint8_t cb) {
        if (count >= Capacity)
            throw std::runtime_error("FixedSizeMap is full");

        std::size_t idx = hash(key) & (Capacity - 1);
        while (table[idx].used) {
            // Update if key already exists.
            if (std::memcmp(table[idx].orderid, key, 20) == 0) {
                table[idx].callback_id = cb;
                return;
            }
            idx = (idx + 1) & (Capacity - 1);
        }
        std::memcpy(table[idx].orderid, key, 20);
        table[idx].callback_id = cb;
        table[idx].used = true;
        ++count;
    }

    // Find the callback_id associated with a key.
    // Returns true if found, false otherwise.
    uint8_t find(const char key[20]) const {
        std::size_t idx = hash(key) & (Capacity - 1);
        for (std::size_t i = 0; i < Capacity; ++i) {
            if (!table[idx].used)
                return 255; // Key not found.
            if (std::memcmp(table[idx].orderid, key, 20) == 0) {
                return table[idx].callback_id;
            }
            idx = (idx + 1) & (Capacity - 1);
        }
        return 255;
    }
};

}} // namespace KTN::Core

#endif // FIXED_SIZE_MAP_HPP
