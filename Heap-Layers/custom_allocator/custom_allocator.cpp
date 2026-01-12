#include <cstring>
#include "custom_policy.h"

#ifndef HL_POLICY
#error "HL_POLICY must be defined"
#endif

using Heap = TheAllocator<HL_POLICY>;
Heap theHeap;

extern "C" {

void* malloc(size_t sz) {
    return theHeap.malloc(sz);
}

void free(void* p) {
    theHeap.free(p);
}

void* realloc(void* p, size_t sz) {
    return theHeap.realloc(p, sz);
}

void* calloc(size_t n, size_t sz) {
    size_t total = n * sz;
    void* p = theHeap.malloc(total);
    if (p) std::memset(p, 0, total);
    return p;
}

}
