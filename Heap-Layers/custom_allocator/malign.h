#include "utility/align.h" // Make sure to include the file you found

// =========================================================================
// CUSTOM ADAPTER: Malign
// Uses HL::align to ensure every allocation is a multiple of 'Alignment'.
// This implicitly enforces a minimum size (e.g. 1 byte becomes 16).
// =========================================================================
template <int Alignment, class Super>
class Malign : public Super {
public:
    inline void* malloc(size_t sz) {
        // HL::align handles the rounding (e.g., 1 -> 16, 13 -> 16, 17 -> 32)
        if (sz < Alignment) sz = Alignment;
        return Super::malloc(HL::align<Alignment>(sz));
    }
    
    // Pass-through free (size doesn't matter for free in this layer)
    inline void free(void* ptr) { 
        Super::free(ptr); 
    }
    
    // Align the size for realloc too
    inline void* realloc(void* ptr, size_t sz) {
        return Super::realloc(ptr, HL::align<Alignment>(sz));
    }
};
