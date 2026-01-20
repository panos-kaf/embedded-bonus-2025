#include "heaplayers.h"

inline size_t class_to_size(const int i) {
    // Base power of 2 (e.g., for i=0 or 1, shift is 3 -> size 8)
    int shift = (i >> 1) + 3;

    size_t size = (size_t)1ULL << shift;

    // If odd index, add 50% (The half-step)
    if (i & 1) {
    size += (size >> 1);
    }

    return size;
}

inline int size_to_class(const size_t sz) {
    // 1. Clamp minimum size to 8 bytes
    size_t s = (sz < 8) ? 8 : sz;

    // 2. Find log2 of (s-1) to handle boundaries correctly
    //    e.g., 16 should be treated as "fits in 16", not "overflows to 32"
    int k = HL::ilog2(s - 1);

    // 3. Calculate the "Half-Step" boundary for this power of 2
    //    Boundary = 1.5 * 2^k
    //    (e.g., if k=4 (16), boundary is 24)
    size_t halfStep = (1ULL << k) + (1ULL << (k - 1));

    // 4. Calculate Base Index: (Log2 - 3) * 2
    //    We multiply by 2 because we have double the bins of Kingsley
    int idx = (k - 3) << 1;

    // 5. Select correct bin
    if (s <= halfStep) {
    idx += 1; // Fits in the 1.5x bin (e.g., 12, 24)
    } else {
    idx += 2; // Needs the next Power of 2 bin (e.g., 16, 32)
    }

    return idx;
}
