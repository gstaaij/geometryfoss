#pragma once
#include "object.h"
#include <stddef.h>

// A dynamic array of Objects
typedef struct {
    Object* items;
    size_t count;
    size_t capacity;
} DAObjects;
