#pragma once

/*
BrunoNotes C helper lib

References:
- nob.h (https://github.com/tsoding/nob.h)
- odin (https://odin-lang.org) - gb.h (https://github.com/gingerBill/gb)
#define BN_IMPLEMENTATION
#include "bn.h"

This library uses a global context called bn_ctx that must be initialized if
used, for example to change the log options.

    bn_initCxt(BN_DEFAULT_CONTEXT_PARAMS);

the bn_ctx contains a temp_allocator, that is basically a arena allocator
that must be freed by the user, at the end of a main loop for example

    bn_allocatorFreeAll(&bn_ctx->temp_allocator);

(optional) destroy context

    bn_destroyCxt();

*/

#define BN_ASSERT_ENABLED // comment to disable asserts

// clang-format off
#if !defined(BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY) && !defined(BN_ALLOCATOR_BACKEND_MALLOC)
    #define BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY
    // #define BN_ALLOCATOR_BACKEND_MALLOC
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define BN_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define BN_PLATFORM_OSX
#elif defined(__linux__)
    #define BN_PLATFORM_LINUX
#endif

#if defined(DEBUG)
    #define BN_DEBUG true
#else
    #define BN_DEBUG false
#endif

// clang-format on

#if defined(__cplusplus)
extern "C" {
#endif

// clang-format off
#ifndef thread_local
    #define thread_local _Thread_local
#endif

#include <stdio.h>

#if defined(__GNUC__)
    #ifndef _VA_LIST_DEFINED
        typedef __gnuc_va_list va_list;
        #define _VA_LIST_DEFINED
    #endif
#else
    #include <stdarg.h>
#endif

#ifndef va_arg
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
        /* C23 uses a special builtin. */
        #define va_start(...) __builtin_c23_va_start(__VA_ARGS__)
    #else
        /* Versions before C23 do require the second parameter. */
        #define va_start(ap, param) __builtin_va_start(ap, param)
    #endif
    #define va_end(ap) __builtin_va_end(ap)
    #define va_arg(ap, T) __builtin_va_arg(ap, T)
#endif

#if _MSC_VER
    #include <intrin.h>
    #define debugBreak() __debugbreak()
#else
    #define debugBreak() __builtin_trap()
#endif
// clang-format on

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8 b8;
typedef i32 b32;
typedef i64 b64;
typedef float f32;
typedef double f64;
typedef size_t usize;
typedef ptrdiff_t isize;

#define bn_vec2Prototype(T)                                                    \
    typedef union U_Vec2##T {                                                  \
        T elements[2];                                                         \
        struct {                                                               \
            union {                                                            \
                T x;                                                           \
            };                                                                 \
            union {                                                            \
                T y;                                                           \
            };                                                                 \
        };                                                                     \
    } Vec2##T;

bn_vec2Prototype(f32);
bn_vec2Prototype(f64);

#define bn_vec3Prototype(T)                                                    \
    typedef union U_Vec3##T {                                                  \
        T elements[3];                                                         \
        struct {                                                               \
            union {                                                            \
                T x, r;                                                        \
            };                                                                 \
            union {                                                            \
                T y, g;                                                        \
            };                                                                 \
            union {                                                            \
                T z, b;                                                        \
            };                                                                 \
        };                                                                     \
    } Vec3##T;

bn_vec3Prototype(f32);

Vec3f32 vec3f32Add(Vec3f32 u, Vec3f32 v);
Vec3f32 vec3f32AddScalar(Vec3f32 u, f32 s);
Vec3f32 vec3f32Sub(Vec3f32 u, Vec3f32 v);
Vec3f32 vec3f32Mult(Vec3f32 u, Vec3f32 v);
Vec3f32 vec3f32MultScalar(Vec3f32 u, f32 s);
Vec3f32 vec3f32DivScalar(Vec3f32 u, f32 s);
f32 vec3f32LengthSquared(Vec3f32 v);
f32 vec3f32Length(Vec3f32 v);
Vec3f32 vec3f32UnitVector(Vec3f32 v);
f32 vec3f32Dot(Vec3f32 u, Vec3f32 v);
Vec3f32 vec3f32Cross(Vec3f32 u, Vec3f32 v);
Vec3f32 vec3f32Negative(Vec3f32 v);

bn_vec3Prototype(f64);

#define bn_vec4Prototype(T)                                                    \
    typedef union U_Vec4##T {                                                  \
        T elements[4];                                                         \
        struct {                                                               \
            union {                                                            \
                T x, r;                                                        \
            };                                                                 \
            union {                                                            \
                T y, g;                                                        \
            };                                                                 \
            union {                                                            \
                T z, b;                                                        \
            };                                                                 \
            union {                                                            \
                T w, a;                                                        \
            };                                                                 \
        };                                                                     \
    } Vec4##T;

bn_vec4Prototype(f32);
bn_vec4Prototype(f64);

#define bn_arrayLen(a) (sizeof(a) / sizeof(a[0]))

typedef struct {
    char* ptr;
    i64 length;
} String;

#define stringLit(s)                                                           \
    (String){(char*)(s), sizeof((s)) - 1} // makes a string from char*
#define stringFmt(s)                                                           \
    (s).length, (s).ptr // to be used with "%.*s" in printf for example

#define Byte 1
#define KiloByte (u64)(1024 * Byte)
#define MegaByte (u64)(1024 * KiloByte)
#define GigaByte (u64)(1024 * MegaByte)
#define TeraByte (u64)(1024 * GigaByte)
#define PetaByte (u64)(1024 * TeraByte)
#define ExaByte (u64)(1024 * PetaByte)

#define Kib(n) ((u64)(n) * KiloByte)
#define Mib(n) ((u64)(n) * MegaByte)
#define Gib(n) ((u64)(n) * GigaByte)

// clang-format off
#if defined(BN_ALLOCATOR_BACKEND_MALLOC)
    // for alignment
    #ifndef BN_ALLOCATOR_MIN_SIZE
        #define BN_ALLOCATOR_MIN_SIZE Kib(4)
    #endif
#endif
// clang-format on

#define bn_min(a, b) (((a) < (b)) ? (a) : (b))
#define bn_max(a, b) (((a) > (b)) ? (a) : (b))
#define bn_abs(x) ((x) < 0 ? -(x) : (x))
#define bn_clamp(x, lower, upper) bn_min(bn_max((x), (lower)), (upper))

#define bn_alignPow2(x, b) (((x) + (b) - 1) & (~((b) - 1)))

#define bn_defer(begin, end)                                                   \
    for (i8 _bn_defer_latch = (begin, 0); _bn_defer_latch < 1;                 \
         _bn_defer_latch = 1, end)

typedef enum {
    BN_LogLevel_Debug = 0,
    BN_LogLevel_Info = 1,
    BN_LogLevel_Warning = 2,
    BN_LogLevel_Error = 3,
    BN_LogLevel_Panic = 4,
} BN_LogLevel;

typedef struct {
    BN_LogLevel level;
    bool terminal_colors;
    bool enable;
} BN_LogOpts;

void bn_defaultLogHandler(BN_LogOpts opts, const char* fmt, va_list args);

typedef void(BN_LogHandler)(BN_LogOpts opts, const char* fmt, va_list args);

void bn_log(BN_LogLevel level, const char* fmt, ...);

#define bn_logInfo(message) bn_log(BN_LogLevel_Info, message)
#define bn_logInfof(...) bn_log(BN_LogLevel_Info, __VA_ARGS__)
#define bn_logWarn(message) bn_log(BN_LogLevel_Warning, message)
#define bn_logWarnf(...) bn_log(BN_LogLevel_Warning, __VA_ARGS__)
#define bn_logError(message) bn_log(BN_LogLevel_Error, message)
#define bn_logErrorf(...) bn_log(BN_LogLevel_Error, __VA_ARGS__)
#define bn_logPanic(message) bn_log(BN_LogLevel_Panic, message)
#define bn_logPanicf(...) bn_log(BN_LogLevel_Panic, __VA_ARGS__)
#define bn_logDebug(message) bn_log(BN_LogLevel_Debug, message)
#define bn_logDebugf(...) bn_log(BN_LogLevel_Debug, __VA_ARGS__)
#define bn_logFatal(message) bn_log(BN_LogLevel_Fatal, message)
#define bn_logFatalf(...) bn_log(BN_LogLevel_Fatal, __VA_ARGS__)

// clang-format off
#if defined(BN_ASSERT_ENABLED)
    #define bn_assert(expr)                                                         \
        do {                                                                       \
            if (expr) {                                                            \
            } else {                                                               \
                bn_logErrorf(                                                       \
                    "Assertion Failure: %s, in file: %s, line: %d", #expr,         \
                    __FILE__, __LINE__                                             \
                );                                                                 \
                debugBreak();                                                      \
            }                                                                      \
        } while (0)

    #define bn_assertMsg(expr, msg)                                                 \
        do {                                                                       \
            if (expr) {                                                            \
            } else {                                                               \
                bn_logErrorf(                                                       \
                    "Assertion Failure: %s, in file: %s, line: %d, msg: %s",       \
                    #expr, __FILE__, __LINE__, msg                                 \
                );                                                                 \
                debugBreak();                                                      \
            }                                                                      \
        } while (0)
#else
    #define bn_assert(expr)
    #define bn_assertMsg(expr, msg)
#endif

// clang-format on

u32 bn_platformGetPageSize(void);
void* bn_platformMemReserve(u64 size);
b32 bn_platformMemCommit(void* ptr, u64 size);
b32 bn_platformMemDecommit(void* ptr, u64 size);
b32 bn_platformMemRelease(void* ptr, u64 size);

#define BN_ARENA_BASE_POS (sizeof(BN_Arena))
#define BN_ARENA_ALIGN (sizeof(void*))

// clang-format off
#if defined (BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY)
    // use malloc for other platforms as fallback
    #if !defined(BN_PLATFORM_LINUX) && !defined(BN_PLATFORM_WINDOWS)
        #undef BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY
        #if !defined(BN_ALLOCATOR_BACKEND_MALLOC)
            #define BN_ALLOCATOR_BACKEND_MALLOC
        #endif
    #endif
#endif
// clang-format on

typedef enum {
    BN_ArentaType_Static,
    BN_ArenaType_Growing,
} BN_ArenaType;

typedef struct {
    u64 reserve_size;
    u64 commit_size;
    u64 pos;
    u64 commit_pos;
    BN_ArenaType type;
} BN_Arena;

BN_Arena* bn_initArena(u64 reserve_size, u64 commit_size, BN_ArenaType type);
void bn_destroyArena(BN_Arena* arena);

void* bn_arenaPush(BN_Arena* arena, u64 size, bool non_zero);
BN_Arena* bn_arenaRealloc(BN_Arena* arena, u64 new_pos);
void bn_arenaPop(BN_Arena* arena, u64 size);
void bn_arenaPopTo(BN_Arena* arena, u64 pos);

typedef struct {
    BN_Arena* arena;
    u64 start_pos;
} BN_TempArena;

BN_TempArena bn_beginTempArena(BN_Arena* arena);
void bn_endTempArena(BN_TempArena temp);

void* bn_heapAllocatorPush(u64 size, bool non_zero);
void bn_heapAllocatorFree(void* data, u64 size);

typedef enum {
    BN_AllocatorType_ArenaStatic,
    BN_AllocatorType_ArenaGrowing,
    BN_AllocatorType_HeapAllocator,
    BN_AllocatorType_TempAllocator,
} BN_AllocatorType;

typedef struct {
    u64 reserve_size;
    u64 commit_size;
    BN_AllocatorType type;
} BN_AllocatorInitParams;

typedef struct {
    u64 size;
    b32 non_zero;
} BN_AllocatorParams;

typedef struct {
    BN_AllocatorType type;
    void* data;
    u64 size;
    BN_TempArena _temp;
} BN_Allocator;

BN_Allocator bn_initAllocator(BN_AllocatorInitParams params);
void bn_destroyAllocator(BN_Allocator* alloc);

void* bn_allocatorPush(BN_AllocatorParams params, BN_Allocator* alloc);
void bn_allocatorFree(BN_Allocator* alloc, void* data);
void bn_allocatorFreeAll(BN_Allocator* alloc);

#define bn_allocPushStruct(alloc, T)                                           \
    (T*)bn_allocatorPush((BN_AllocatorParams){sizeof(T), false}, (alloc))
#define bn_allocPushStructNZ(alloc, T)                                         \
    (T*)bn_allocatorPush((BN_AllocatorParams){sizeof(T), true}, (alloc))
#define bnAllocPushArray(alloc, T, len)                                        \
    (T*)bn_allocatorPush(                                                      \
        (BN_AllocatorParams){sizeof(T) * (len), false}, (alloc)                \
    )
#define bnAllocPushArrayNZ(alloc, T, len)                                      \
    (T*)bn_allocatorPush((BN_AllocatorParams){sizeof(T) * (len), true}, (alloc))

#define bn_dArrayPrototype(T)                                                  \
    typedef struct {                                                           \
        T* items;                                                              \
        u32 length;                                                            \
        u32 capacity;                                                          \
    } BN_array##T

bn_dArrayPrototype(i8);
bn_dArrayPrototype(i16);
bn_dArrayPrototype(i32);
bn_dArrayPrototype(i64);
bn_dArrayPrototype(u8);
bn_dArrayPrototype(u16);
bn_dArrayPrototype(u32);
bn_dArrayPrototype(u64);
bn_dArrayPrototype(b8);
bn_dArrayPrototype(b32);
bn_dArrayPrototype(b64);
bn_dArrayPrototype(f32);
bn_dArrayPrototype(f64);
bn_dArrayPrototype(String);
bn_dArrayPrototype(Vec2f32);
bn_dArrayPrototype(Vec2f64);
bn_dArrayPrototype(Vec3f32);
bn_dArrayPrototype(Vec3f64);
bn_dArrayPrototype(Vec4f32);
bn_dArrayPrototype(Vec4f64);

#ifndef BN_DYNAMIC_ARRAY_INITIAL_CAPACITY
#define BN_DYNAMIC_ARRAY_INITIAL_CAPACITY 256
#endif

#define bn_daInit() {.items = NULL, .length = 0, .capacity = 0}

#define bn_daReserve(da, expected_capacity, allocator)                         \
    do {                                                                       \
        if ((expected_capacity) > (da)->capacity) {                            \
            if ((da)->capacity == 0) {                                         \
                (da)->capacity = BN_DYNAMIC_ARRAY_INITIAL_CAPACITY;            \
                (da)->items = bn_allocatorPush(                                \
                    (BN_AllocatorParams){                                      \
                        (da)->capacity * sizeof(*(da)->items), false           \
                    },                                                         \
                    (allocator)                                                \
                );                                                             \
            } else {                                                           \
                u32 old_capacity = (da)->capacity;                             \
                while ((expected_capacity) > (da)->capacity) {                 \
                    (da)->capacity *= 2;                                       \
                }                                                              \
                void* new_entries = bn_allocatorPush(                          \
                    (BN_AllocatorParams){                                      \
                        (da)->capacity * sizeof(*(da)->items), false           \
                    },                                                         \
                    (allocator)                                                \
                );                                                             \
                memcpy(                                                        \
                    new_entries, (da)->items,                                  \
                    old_capacity * sizeof(*(da)->items)                        \
                );                                                             \
                bn_allocatorFree((allocator), (da)->items);                    \
                (da)->items = new_entries;                                     \
            }                                                                  \
            bn_assert((da)->items != NULL);                                    \
        }                                                                      \
    } while (0)

#define bn_daAppend(da, item, allocator)                                       \
    do {                                                                       \
        bn_daReserve((da), (da)->length + 1, (allocator));                     \
        (da)->items[(da)->length++] = (item);                                  \
    } while (0)

#define bn_daAppendMany(da, new_items, new_items_count, allocator)             \
    do {                                                                       \
        bn_daReserve((da), (da)->length + (new_items_count), (allocator));     \
        memcpy(                                                                \
            (da)->items + (da)->length, (new_items),                           \
            (new_items_count) * sizeof(*(da)->items)                           \
        );                                                                     \
        (da)->length += (new_items_count);                                     \
    } while (0)

#define bn_daFree(da, allocator)                                               \
    do {                                                                       \
        bn_allocatorFree((allocator), (da)->items);                            \
        (da)->length = 0;                                                      \
        (da)->capacity = 0;                                                    \
    } while (0)

#define bn_daResize(da, new_size, allocator)                                   \
    do {                                                                       \
        bn_daReserve((da), new_size, (allocator));                             \
        (da)->length = (new_size);                                             \
    } while (0)

#ifndef BN_HASH_TABLE_INITIAL_CAPACITY
#define BN_HASH_TABLE_INITIAL_CAPACITY 256
#endif

typedef struct {
    String key;
    void* value;
} BN_HashTableEntry;

typedef struct {
    BN_HashTableEntry* items;
    u32 length;
    u32 capacity;
} BN_HashTable;

#define bn_htInit() {.items = NULL, .length = 0, .capacity = 0}

void bn_hashTableReserve(
    BN_HashTable* table, u64 expected_capacity, BN_Allocator* allocator
);
void bn_hashTableFree(BN_HashTable* table, BN_Allocator* allocator);
u32 bn_hashTableLinearProbe(BN_HashTable* table, String key);
void* bn_hashTableGet(BN_HashTable* table, String key);
void bn_hashTableAppend(
    BN_HashTable* table, String key, void* value, BN_Allocator* allocator
);

#define bn_htAppend(ht, key, value, allocator)                                 \
    bn_hashTableAppend(ht, stringLit(key), (void*)value, (allocator))

#define bn_htGet(ht, key) bn_hashTableGet(ht, stringLit(key))

typedef struct {
    BN_Allocator allocator;
    BN_Allocator temp_allocator;
    BN_LogHandler* log_handler;
    BN_LogOpts log_opts;
} BN_Context;

typedef struct {
    BN_AllocatorInitParams allocator_params;
    BN_LogHandler* log_handler;
    BN_LogOpts log_opts;
} BN_ContextInitParams;

#define BN_DEFAULT_CONTEXT_PARAMS                                              \
    (BN_ContextInitParams) {                                                   \
        .allocator_params =                                                    \
            (BN_AllocatorInitParams){                                          \
                .type = BN_AllocatorType_HeapAllocator,                        \
            },                                                                 \
        .log_opts = (BN_LogOpts) {                                             \
            .level = BN_DEBUG ? BN_LogLevel_Debug : BN_LogLevel_Info,          \
            .terminal_colors = BN_IS_TERMINAL, .enable = true,                 \
        }                                                                      \
    }

// only used to initialize the context on the heap
static thread_local BN_Context* bn_ctx = NULL;

void bn_initCxt(BN_ContextInitParams params);
void bn_destroyCtx();

// ----------------------------------------------------------------------------
// init BN_IMPLEMENTATION
// ----------------------------------------------------------------------------

#if defined(BN_IMPLEMENTATION)

#include <stdlib.h>
#include <string.h>

// clang-format off
#if defined(BN_PLATFORM_WINDOWS)
    #include <windows.h>
    #include <io.h>

    #define BN_IS_TERMINAL _isatty(_fileno(stdin))
#else
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>

    #define BN_IS_TERMINAL isatty(STDOUT_FILENO)
#endif
// clang-format on

#define BN_ANSI_ESC "\e"
// #define BN_ANSI_ESC "\x1b"
#define BN_ANSI_CSI BN_ANSI_ESC "["
#define BN_ANSI_OSC BN_ANSI_ESC "]"
#define BN_ANSI_SGR "m"

#define BN_ANSI_RESET BN_ANSI_CSI "0" BN_ANSI_SGR
#define BN_ANSI_DARK_GREY BN_ANSI_CSI "90" BN_ANSI_SGR
#define BN_ANSI_YELLOW BN_ANSI_CSI "33" BN_ANSI_SGR
#define BN_ANSI_RED BN_ANSI_CSI "33" BN_ANSI_SGR
#define BN_ANSI_PURPLE BN_ANSI_CSI "35" BN_ANSI_SGR

void bn_defaultLogHandler(BN_LogOpts opts, const char* fmt, va_list args) {
    if (bn_ctx != NULL) {
        if (opts.level < bn_ctx->log_opts.level)
            return;
    } else {
        if (opts.level < BN_LogLevel_Info)
            return;
    }

    if (!opts.enable)
        return;

    switch (opts.level) {
    case BN_LogLevel_Debug:
        if (opts.terminal_colors)
            fprintf(stderr, BN_ANSI_DARK_GREY);

        fprintf(stderr, "[DEBUG]");
        break;
    case BN_LogLevel_Info:
        if (opts.terminal_colors)
            fprintf(stderr, BN_ANSI_RESET);

        fprintf(stderr, "[INFO ]");
        break;
    case BN_LogLevel_Warning:
        if (opts.terminal_colors)
            fprintf(stderr, BN_ANSI_YELLOW);

        fprintf(stderr, "[WARN ]");
        break;
    case BN_LogLevel_Error:
        if (opts.terminal_colors)
            fprintf(stderr, BN_ANSI_RED);

        fprintf(stderr, "[ERROR]");
        break;
    case BN_LogLevel_Panic:
        if (opts.terminal_colors)
            fprintf(stderr, BN_ANSI_PURPLE);

        fprintf(stderr, "[PANIC]");
        break;
    default:
        return;
    }

    if (opts.terminal_colors)
        fprintf(stderr, BN_ANSI_RESET);

    fprintf(stderr, " ");

    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\r\n");

    if (opts.level == BN_LogLevel_Panic) {
        exit(1);
    }
}

void bn_log(BN_LogLevel level, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    if (bn_ctx != NULL && bn_ctx->log_handler != NULL) {
        bn_ctx->log_handler(
            (BN_LogOpts){
                .level = level,
                .terminal_colors = bn_ctx->log_opts.terminal_colors,
                .enable = bn_ctx->log_opts.enable,
            },
            fmt, args
        );
    } else {
        bn_defaultLogHandler(
            (BN_LogOpts){
                .level = level,
                .terminal_colors = BN_IS_TERMINAL,
                .enable = true,
            },
            fmt, args
        );
    }

    va_end(args);
}

#if defined(BN_PLATFORM_WINDOWS)

static void bn_printLastWindowsError(const char* contextMessage) {
    DWORD error_message_id = GetLastError();
    if (error_message_id == 0) {
        return; // No error occurred
    }

    LPSTR message_buffer = NULL;

    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error_message_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message_buffer, 0, NULL
    );

    if (size > 0) {
        bn_logErrorf(
            "%s Error %lu: %s", contextMessage, error_message_id, message_buffer
        );
    } else {
        bn_logErrorf(
            "%s Error %lu (Failed to format message)\n", contextMessage,
            error_message_id
        );
    }

    LocalFree(message_buffer);
}

u32 bn_platformGetPageSize(void) {
    SYSTEM_INFO sysinfo = {0};
    GetSystemInfo(&sysinfo);

    return sysinfo.dwPageSize;
}

void* bn_platformMemReserve(u64 size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

b32 bn_platformMemCommit(void* ptr, u64 size) {
    void* ret = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
    return ret != NULL;
}

b32 bn_platformMemDecommit(void* ptr, u64 size) {
    b32 result = VirtualFree(ptr, size, size > 0 ? MEM_DECOMMIT : MEM_RELEASE);

    if (result <= 0) {
        bn_printLastWindowsError("Error bn_platformMemDecommit");
    }

    return result;
}

b32 bn_platformMemRelease(void* ptr, u64 size) {
    b32 result = VirtualFree(ptr, size, size > 0 ? MEM_DECOMMIT : MEM_RELEASE);

    if (result <= 0) {
        bn_printLastWindowsError("Error bn_platformMemRelease");
    }

    return result;
}

#elif defined(BN_PLATFORM_LINUX)

u32 bn_platformGetPageSize(void) {
    return (u32)sysconf(_SC_PAGESIZE);
}

void* bn_platformMemReserve(u64 size) {
    void* out = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (out == MAP_FAILED) {
        return NULL;
    }
    return out;
}

b32 bn_platformMemCommit(void* ptr, u64 size) {
    i32 ret = mprotect(ptr, size, PROT_READ | PROT_WRITE);
    return ret == 0;
}

b32 bn_platformMemDecommit(void* ptr, u64 size) {
    i32 ret = mprotect(ptr, size, PROT_NONE);
    if (ret != 0)
        return false;
    ret = madvise(ptr, size, MADV_DONTNEED);
    return ret == 0;
}

b32 bn_platformMemRelease(void* ptr, u64 size) {
    i32 ret = munmap(ptr, size);
    return ret == 0;
}

#endif

BN_Arena* bn_initArena(u64 reserve_size, u64 commit_size, BN_ArenaType type) {
    bn_assert(reserve_size > 0);
    bn_assert(commit_size > 0);

#if defined(BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY)
    u32 page_size = bn_platformGetPageSize();

    reserve_size = bn_alignPow2(reserve_size, page_size);
    commit_size = bn_alignPow2(commit_size, page_size);

    BN_Arena* arena = (BN_Arena*)bn_platformMemReserve(reserve_size);

    if (!bn_platformMemCommit(arena, commit_size)) {
        return NULL;
    }
#elif defined(BN_ALLOCATOR_BACKEND_MALLOC)
    reserve_size = bn_alignPow2(reserve_size, BN_ALLOCATOR_MIN_SIZE);
    BN_Arena* arena = (BN_Arena*)malloc(reserve_size);
    bn_assert(arena != NULL);
#endif

    arena->reserve_size = reserve_size;
    arena->commit_size = commit_size;
    arena->pos = BN_ARENA_BASE_POS;
    arena->commit_pos = commit_size;
    arena->type = type;

    return arena;
}

void bn_destroyArena(BN_Arena* arena) {
#if defined(BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY)
    bool ok = bn_platformMemRelease(arena, arena->reserve_size);
    bn_assert(ok == true);
#elif defined(BN_ALLOCATOR_BACKEND_MALLOC)
    free(arena);
#endif
}

void* bn_arenaPush(BN_Arena* arena, u64 size, bool non_zero) {
    u64 pos_aligned = bn_alignPow2(arena->pos, BN_ARENA_ALIGN);
    u64 new_pos = pos_aligned + size;

    // Grow arena if needed
    if (new_pos > arena->reserve_size && arena->type == BN_ArenaType_Growing) {
        arena = bn_arenaRealloc(arena, new_pos);
    }

    if (new_pos > arena->commit_pos) {
        u64 new_commit_pos = new_pos;
        new_commit_pos += arena->commit_size - 1;
        new_commit_pos -= new_commit_pos % arena->commit_size;
        new_commit_pos = bn_min(new_commit_pos, arena->reserve_size);

#if defined(BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY)
        u8* mem = (u8*)arena + arena->commit_pos;
        u64 commit_size = new_commit_pos - arena->commit_pos;

        if (!bn_platformMemCommit(mem, commit_size)) {
            return NULL;
        }
#endif

        arena->commit_pos = new_commit_pos;
    }

    arena->pos = new_pos;

    u8* out = (u8*)arena + pos_aligned;

    if (!non_zero) {
        memset(out, 0, size);
    }

    return out;
}

BN_Arena* bn_arenaRealloc(BN_Arena* arena, u64 new_pos) {
    u64 new_reserve_size = arena->reserve_size * 2;
    while (new_reserve_size < new_pos) {
        new_reserve_size *= 2;
    }
    u64 commit_size = arena->commit_size;
    u64 pos = arena->pos;
    u64 commit_pos = arena->commit_pos;

#if defined(BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY)
    u32 page_size = bn_platformGetPageSize();
    new_reserve_size = bn_alignPow2(new_reserve_size, page_size);

    BN_Arena* new_arena = (BN_Arena*)bn_platformMemReserve(new_reserve_size);
    bn_assert(new_arena != NULL);

    u64 bytes_to_commit = commit_pos;
    if (bytes_to_commit < sizeof(BN_Arena)) {
        bytes_to_commit = sizeof(BN_Arena);
    }
    bytes_to_commit = bn_alignPow2(bytes_to_commit, page_size);

    if (!bn_platformMemCommit(new_arena, arena->commit_pos)) {
        bn_platformMemRelease(new_arena, new_reserve_size);
        return NULL;
    }
    // Copy existing data and commit the current committed size
    memcpy(new_arena, arena, arena->commit_pos);
    // if (!bn_platformMemCommit(new_arena, arena->commit_pos)) {
    //     bn_platformMemRelease(new_arena, new_reserve_size);
    //     return NULL;
    // }
    new_arena->reserve_size = new_reserve_size;
    new_arena->commit_size = commit_size;
    new_arena->pos = pos;
    new_arena->commit_pos = commit_pos;

    bn_platformMemRelease(arena, arena->reserve_size);
#elif defined(BN_ALLOCATOR_BACKEND_MALLOC)
    new_reserve_size = bn_alignPow2(new_reserve_size, BN_ALLOCATOR_MIN_SIZE);
    // BN_Arena* new_arena =
    //     (BN_Arena*)malloc(new_reserve_size);
    // bn_assert(new_arena != NULL);
    //
    // memcpy(new_arena, arena, arena->commit_pos);
    //
    // free(arena);
    BN_Arena* new_arena = (BN_Arena*)realloc(arena, new_reserve_size);

    new_arena->reserve_size = new_reserve_size;
    new_arena->commit_size = commit_size;
    new_arena->pos = pos;
    new_arena->commit_pos = commit_pos;
#endif

    return new_arena;
}

void bn_arenaPop(BN_Arena* arena, u64 size) {
    size = bn_min(size, arena->pos - BN_ARENA_BASE_POS);
    arena->pos -= size;
}

void bn_arenaPopTo(BN_Arena* arena, u64 pos) {
    u64 size = pos < arena->pos ? arena->pos - pos : 0;
    bn_arenaPop(arena, size);
}

BN_TempArena bn_beginTempArena(BN_Arena* arena) {
    return (BN_TempArena){
        .arena = arena,
        .start_pos = arena->pos,
    };
}

void bn_endTempArena(BN_TempArena temp) {
    bn_arenaPopTo(temp.arena, temp.start_pos);
}

void* bn_heapAllocatorPush(u64 size, bool non_zero) {
#if defined(BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY)
    u32 page_size = bn_platformGetPageSize();
    size = bn_alignPow2(size, page_size);
    void* data = bn_platformMemReserve(size);

    if (!bn_platformMemCommit(data, size)) {
        return NULL;
    }
#elif defined(BN_ALLOCATOR_BACKEND_MALLOC)
    size = bn_alignPow2(size, BN_ALLOCATOR_MIN_SIZE);
    void* data = malloc(size);
#endif

    if (!non_zero) {
        memset(data, 0, size);
    }

    return data;
}

void bn_heapAllocatorFree(void* data, u64 size) {
#if defined(BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY)
    bn_platformMemRelease(data, size);
#elif defined(BN_ALLOCATOR_BACKEND_MALLOC)
    (void)size;
    free(data);
    data = NULL;
#endif
}

BN_Allocator bn_initAllocator(BN_AllocatorInitParams params) {
    BN_Allocator alloc = {0};

    switch (params.type) {
    case BN_AllocatorType_ArenaStatic:
        alloc.data = bn_initArena(
            params.reserve_size, params.commit_size, BN_ArentaType_Static
        );
        alloc.type = params.type;
        break;
    case BN_AllocatorType_ArenaGrowing:
        if (params.reserve_size <= 0) {
            params.reserve_size = Byte * 8;
        }
        if (params.commit_size <= 0) {
            params.commit_size = Byte * 8;
        }

        alloc.data = bn_initArena(
            params.reserve_size, params.commit_size, BN_ArenaType_Growing
        );
        alloc.type = params.type;
        break;
    case BN_AllocatorType_HeapAllocator:
        alloc.data = NULL;
        alloc.size = 0;
        alloc.type = params.type;
        break;
    case BN_AllocatorType_TempAllocator:
        alloc.data = bn_initArena(
            params.reserve_size, params.commit_size, BN_ArenaType_Growing
        );
        alloc.type = params.type;
        alloc._temp = bn_beginTempArena((BN_Arena*)alloc.data);
        break;
    default:
        bn_logPanic("Allocator not implemented");
        break;
    }

    return alloc;
}

void bn_destroyAllocator(BN_Allocator* alloc) {
    switch (alloc->type) {
    case BN_AllocatorType_ArenaStatic:
    case BN_AllocatorType_ArenaGrowing:
        bn_logDebugf("BN: destroying arena allocator");

        bn_destroyArena((BN_Arena*)alloc->data);
        break;
    case BN_AllocatorType_HeapAllocator:
        break;
    case BN_AllocatorType_TempAllocator:
        bn_logDebugf("BN: destroying temp allocator");

        bn_endTempArena(alloc->_temp);
        bn_destroyArena((BN_Arena*)alloc->data);
        break;
    default:
        bn_logPanic("Allocator not implemented");
        break;
    }
}

void* bn_allocatorPush(BN_AllocatorParams params, BN_Allocator* alloc) {
    alloc->size = params.size;

    switch (alloc->type) {
    case BN_AllocatorType_ArenaStatic:
    case BN_AllocatorType_ArenaGrowing: {
        // checks reallocation outside the arena push to update the pointer
        BN_Arena* arena = alloc->data;
        u64 pos_aligned = bn_alignPow2(arena->pos, BN_ARENA_ALIGN);
        u64 new_pos = pos_aligned + params.size;
        if (new_pos > arena->reserve_size) {
            arena = bn_arenaRealloc(arena, new_pos);
        }
        alloc->data = arena;
        return bn_arenaPush(
            (BN_Arena*)alloc->data, params.size, params.non_zero
        );
    }
    case BN_AllocatorType_HeapAllocator:
        return bn_heapAllocatorPush(params.size, params.non_zero);
    case BN_AllocatorType_TempAllocator:
        return bn_arenaPush(
            (BN_Arena*)alloc->_temp.arena, params.size, params.non_zero
        );
    default:
        bn_logPanic("Allocator not initialized");
        return NULL;
    }
}

void bn_allocatorFree(BN_Allocator* alloc, void* data) {
    switch (alloc->type) {
    case BN_AllocatorType_ArenaStatic:
    case BN_AllocatorType_ArenaGrowing:
    case BN_AllocatorType_TempAllocator:
        break;
    case BN_AllocatorType_HeapAllocator:
        bn_heapAllocatorFree(data, alloc->size);
        alloc->size = 0;
        break;
    default:
        bn_logPanic("Allocator not initialized");
        break;
    }
}

void bn_allocatorFreeAll(BN_Allocator* alloc) {
    switch (alloc->type) {
    case BN_AllocatorType_ArenaStatic:
    case BN_AllocatorType_ArenaGrowing:
        bn_arenaPopTo((BN_Arena*)alloc->data, BN_ARENA_BASE_POS);
        break;
    case BN_AllocatorType_HeapAllocator:
        break;
    case BN_AllocatorType_TempAllocator:
        bn_endTempArena(alloc->_temp);
        alloc->_temp = bn_beginTempArena((BN_Arena*)alloc->data);
        break;
    default:
        bn_logPanic("Allocator not initialized");
        break;
    }
    alloc->size = 0;
}

static inline u32 bn_fvn32aHash(u8* data, u32 length) {
    u32 h = 0x811c9dc5;

    for (u32 i = 0; i < length; i++) {
        h = (h ^ (u32)data[i]) * 0x01000193;
    }

    return h;
}

void bn_hashTableReserve(
    BN_HashTable* table, u64 expected_capacity, BN_Allocator* allocator
) {
    if (expected_capacity > table->capacity) {
        if (table->capacity == 0) {
            table->capacity = BN_HASH_TABLE_INITIAL_CAPACITY;
            table->items = (BN_HashTableEntry*)bn_allocatorPush(
                (BN_AllocatorParams){
                    table->capacity * sizeof(*table->items), false
                },
                allocator
            );
        } else {
            while (expected_capacity > table->capacity) {
                table->capacity *= 2;
            }
            BN_HashTableEntry* new_entries =
                (BN_HashTableEntry*)bn_allocatorPush(
                    (BN_AllocatorParams){
                        table->capacity * sizeof(*table->items), false
                    },
                    allocator
                );

            for (u32 i = 0; i < table->capacity; i++) {
                if (table->items[i].key.ptr != NULL) {
                    new_entries[i] = table->items[i];
                }
            }

            bn_allocatorFree(allocator, table->items);
            table->items = new_entries;
        }
        bn_assert(table->items != NULL);
    }
}

void bn_hashTableFree(BN_HashTable* table, BN_Allocator* allocator) {
    bn_allocatorFree(allocator, table->items);
    table->length = 0;
    table->capacity = 0;
}

u32 bn_hashTableLinearProbe(BN_HashTable* table, String key) {
    u32 hash = bn_fvn32aHash((u8*)key.ptr, key.length);
    u32 idx = (u32)(hash & (u32)(table->capacity - 1));

    while (table->items[idx].key.ptr != NULL) {
        if (strcmp(key.ptr, table->items[idx].key.ptr) == 0) {
            // Found key
            return idx;
        }
        // Key wasn't in this slot, move to next (linear probing).
        idx++;
        if (idx >= table->capacity) {
            // At end of entries array, wrap around.
            idx = 0;
        }
    }
    return idx;
}

void* bn_hashTableGet(BN_HashTable* table, String key) {
    u32 idx = bn_hashTableLinearProbe(table, key);

    return table->items[idx].value;
}

void bn_hashTableAppend(
    BN_HashTable* table, String key, void* value, BN_Allocator* allocator
) {
    bn_assert(value != NULL);

    bn_hashTableReserve(table, table->length + 1, allocator);

    u32 idx = bn_hashTableLinearProbe(table, key);

    if (table->items[idx].key.ptr == NULL) {
        table->items[idx].key = key;
    }
    table->items[idx].value = value;
    table->length++;
}

#include <math.h>

Vec3f32 vec3f32Add(Vec3f32 u, Vec3f32 v) {
    return (Vec3f32){{u.x + v.x, u.y + v.y, u.z + v.z}};
}

Vec3f32 vec3f32AddScalar(Vec3f32 u, f32 s) {
    return (Vec3f32){{u.x + s, u.y + s, u.z + s}};
}

Vec3f32 vec3f32Sub(Vec3f32 u, Vec3f32 v) {
    return (Vec3f32){{u.x - v.x, u.y - v.y, u.z - v.z}};
}

Vec3f32 vec3f32Mult(Vec3f32 u, Vec3f32 v) {
    return (Vec3f32){{u.x * v.x, u.y * v.y, u.z * v.z}};
}

Vec3f32 vec3f32MultScalar(Vec3f32 u, f32 s) {
    return (Vec3f32){{u.x * s, u.y * s, u.z * s}};
}

Vec3f32 vec3f32DivScalar(Vec3f32 u, f32 s) {
    return (Vec3f32)vec3f32MultScalar(u, (1 / s));
}

f32 vec3f32LengthSquared(Vec3f32 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

f32 vec3f32Length(Vec3f32 v) {
    return sqrt(vec3f32LengthSquared(v));
}

Vec3f32 vec3f32UnitVector(Vec3f32 v) {
    return vec3f32DivScalar(v, vec3f32Length(v));
}

f32 vec3f32Dot(Vec3f32 u, Vec3f32 v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vec3f32 vec3f32Cross(Vec3f32 u, Vec3f32 v) {
    return (Vec3f32){
        {u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x}
    };
}

Vec3f32 vec3f32Negative(Vec3f32 v) {
    return (Vec3f32){
        .x = -v.x,
        .y = -v.y,
        .z = -v.z,
    };
}

void bn_initCxt(BN_ContextInitParams params) {
    if (bn_ctx != NULL)
        return;

    bn_ctx = bn_heapAllocatorPush(sizeof(BN_Context), false);

    bn_ctx->log_opts = params.log_opts;

    if (params.log_handler == NULL) {
        bn_ctx->log_handler = &bn_defaultLogHandler;
        bn_logDebugf("BN: default log handler loaded");
    } else {
        bn_ctx->log_handler = params.log_handler;
    }

#if defined(BN_ALLOCATOR_BACKEND_VIRTUAL_MEMORY)
    bn_logDebugf("BN: allocator backend: virtual memory");
#elif defined(BN_ALLOCATOR_BACKEND_MALLOC)
    bn_logDebugf("BN: allocator backend: malloc");
#endif

    bn_ctx->allocator = bn_initAllocator(params.allocator_params);

    bn_ctx->temp_allocator = bn_initAllocator((BN_AllocatorInitParams){
        .reserve_size = Byte * 8,
        .commit_size = Byte * 8,
        .type = BN_AllocatorType_TempAllocator,
    });
}

void bn_destroyCtx() {
    if (bn_ctx == NULL)
        return;

    bn_destroyAllocator(&bn_ctx->temp_allocator);
    bn_destroyAllocator(&bn_ctx->allocator);

    bn_logDebugf("BN: destroying context");

    bn_heapAllocatorFree(bn_ctx, sizeof(BN_Context));
}

#endif

#if defined(__cplusplus)
}
#endif
