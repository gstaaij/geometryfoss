#pragma once
#include <stdint.h>
#include "object.h"

// The location of the block in grid spaces ( floor(gdcoord/30) )
typedef struct {
    int32_t x, y;
} HMObjectLocation;

typedef struct {
    int64_t key;
    // A dynamic array of objects
    Object* value;
} HMObjects;

int64_t convertObjectLocationToKey(HMObjectLocation key);

// This isn't working because of the deallocation of the pointer. I will probably have to come up with a different solution for optimizing the Objects array
#define addToObjectHashmap(hashmap, key, val) do {      \
        int64_t _key = convertObjectLocationToKey(key); \
        if (hmgeti(hashmap, _key) < 0) {                \
            Object* newArray = NULL;                    \
            arrput(newArray, (val));                    \
            hmput(hashmap, _key, newArray);             \
        } else {                                        \
            arrput(hmget(hashmap, _key), (val));        \
        }                                               \
    } while (0)

Object* getFromObjectHashmap(HMObjects* hashmap, HMObjectLocation key);
