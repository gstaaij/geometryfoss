#pragma once
#include "object.h"
#include <stddef.h>

typedef struct {
    Object* items;
    size_t count;
    size_t capacity;
} DAObjects;
