#include <cstring>
// #include "custom_policy.h"
#include "kingsley-modifications.h"

#ifndef HL_POLICY
#error "HL_POLICY must be defined"
#endif

using Heap = TheAllocator<HL_POLICY>;

// Heap theHeap; 

// This wrapper ensures 'theHeap' is fully constructed before it is ever used.
static Heap& getHeap() {
    static Heap theHeap;
    return theHeap;
}

// Force initialization of heap when library loads
__attribute__((constructor(101)))
static void initHeap() {
    getHeap();
}

extern "C" {

void* malloc(size_t sz) {
    return getHeap().malloc(sz);
}

void free(void* p) {
    getHeap().free(p);
}

void* realloc(void* p, size_t sz) {
    return getHeap().realloc(p, sz);
}

void* calloc(size_t n, size_t sz) {
    size_t total = n * sz;
    void* p = getHeap().malloc(total);
    if (p) std::memset(p, 0, total);
    return p;
}

}
