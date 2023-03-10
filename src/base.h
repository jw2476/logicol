#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>

#include "log.h"

#define CLEAR(object) memset(&object, 0, sizeof(object))
#define LENGTH(array) sizeof(array) / sizeof(*array)

#define DEFINE_RESULT(T, E, NAME) \
typedef struct NAME ## _t { \
    bool ok; \
    union { \
        T result; \
        E error; \
    }; \
} NAME

#define OK(data) { .ok = true, .result = data }

#define ERR(err) { .ok = false, .error = err }

#define DEFINE_OPTIONAL(T, NAME) \
typedef struct NAME ## _t { \
    bool some; \
    T data; \
} NAME

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float   f32;
typedef double  f64;

void init();

#define DARK (Color){ 24, 24, 27, 255 }
#define COMPONENT (Color){ 139, 92, 246, 255 }
#define WIRE (Color){ 113, 113, 122, 255 }
#define CONNECTION (Color){ 14, 165, 233, 255 }
#define ON (Color){ 16, 185, 129, 255 }
#define OFF (Color){ 239, 68, 68, 255 }

