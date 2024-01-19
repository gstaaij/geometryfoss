#include "hashmaps.h"
#include "stb_ds.h"

// Please ignore this file
// Please ignore this file
// Please ignore this file
// Please ignore this file
// Please ignore this file for now
// Please ignore this file
// Please ignore this file
// Please ignore this file
// Please ignore this file
// Please ignore this file

#ifdef __GNUC__
// Disable stupid warnings that complain about some things in stb_ds.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif // __GNUC__

int64_t convertObjectLocationToKey(HMObjectLocation key) {
    int64_t newKey = key.x;
    newKey = (newKey << 32) | key.y;
    return newKey;
}

Object* getFromObjectHashmap(HMObjects* hashmap, HMObjectLocation key) {
    int64_t _key = convertObjectLocationToKey(key);
    if (hmgeti(hashmap, _key) < 0)
        return NULL;
    return hmget(hashmap, _key);
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__