#pragma once
#include "heaplayers.h"
#include <mutex>

// =========================================================================
// 1. RAW SOURCES (Where bytes come from)
// =========================================================================

// Infinite Memory (The "City Water Main")
using SourceMmap = HL::MmapHeap; 

// Fixed Memory (The "Bucket")
template <size_t Size = 1024 * 1024>
using SourceStatic = HL::StaticBufferHeap<Size>;


// =========================================================================
// 2. LAYOUTS (How we organize bytes)
// =========================================================================

// Strategy A: The "Cookie Cutter" (Zone) - Fast, No free()
template <class Source, int ChunkSize = 64 * 1024>
using LayoutZone = HL::SizeHeap<HL::ZoneHeap<Source, ChunkSize>>;

// Strategy B: The "Organizer" (Freelist) - Standard malloc
template <class Source>
using LayoutFreelist = HL::FreelistHeap<Source>;


// =========================================================================
// 3. POLICIES (The Traffic Cops) - ** NEW SECTION **
// =========================================================================

// Router A: The "Hybrid Splitter" (SizeHeap)
// Routes small allocs to 'SmallHeap', large allocs to 'BigHeap'.
// Threshold is usually 256 or 1024 bytes.
template <size_t size, class SmallHeap, class BigHeap>
using PolicySplitter = HL::HybridHeap<size, SmallHeap, BigHeap>;

// Router B: The "Perfect Fit" (SegregatedHeap)
// Creates an array of heaps (one for size 8, one for 16, etc).
// This is used for high-performance "Bucket" allocators.
// 'Map' defines the buckets (e.g., SizeLog2Policy or SizeLinearPolicy).

// not implemented currently
template <int NUM_BINS, int (*getSizeClass) (const size_t), 
          size_t (*getClassMaxSize) (const int), class SmallHeap, class BigHeap>
using PolicyBuckets = HL::SegHeap<
    NUM_BINS,             // number of bins
    getSizeClass,         // size -> class
    getClassMaxSize,      // class -> max size
    SmallHeap,            // LittleHeap
    BigHeap               // BigHeap (fallback: same heap type)
>;

// =========================================================================
// 4. ADAPTERS (Concurrency & Safety)
// =========================================================================

// Thread Safety: Global Lock
template <class Heap>
using AdapterLocked = HL::LockedHeap<std::recursive_mutex, Heap>;

// Thread Safety: Per-Thread Cache
template <class Heap>
using AdapterPerThread = HL::ThreadSpecificHeap<Heap>;


// =========================================================================
// 5. PRE-BUILT RECIPES
// =========================================================================

// RECIPE 1: The "Embedded" (Simple)
using AllocatorEmbedded = AdapterLocked<LayoutFreelist<SourceStatic<1024*1024>>>;

// RECIPE 2: The "Scratchpad" (Fastest)
using AllocatorScratchpad = AdapterPerThread<LayoutZone<SourceMmap>>;

// --- NEW RECIPES USING POLICIES ---

// RECIPE 3: The "Smart Hybrid" (SizeHeap)
// Scenario: We want fast Zones for small items, but we don't want to 
// waste memory for large items.
// 1. Define the two paths
using _SmallPath = LayoutZone<SourceMmap>;      // Fast!
using _BigPath   = LayoutFreelist<SourceMmap>;  // Smart!
// 2. Combine them (Split at 256 bytes)
using _Hybrid    = PolicySplitter<256, _SmallPath, _BigPath>;
// 3. Lock it
using AllocatorHybridLocked = AdapterLocked<_Hybrid>;
using AllocatorHybridPerThread = AdapterPerThread<_Hybrid>;


// RECIPE 4: The "Pro Tier" (Segregated + PerThread)
// This mimics Mimalloc / TCMalloc architecture.
// 1. The Global Source (Locked once per 64KB chunk)
using _Global = AdapterLocked<SourceMmap>;
// 2. The Page Factory (Zones that pull from Global)
using _SmallHeap   = LayoutZone<_Global>;
using _BigHeap     = LayoutFreelist<_Global>;
// 3. The Buckets (Array of Pages)
//    SizeLinearPolicy means buckets are 8, 16, 24, 32...
using _Buckets = PolicyBuckets<
    256,                              // NumBins
    Kingsley::size2Class, // size -> class
    Kingsley::class2Size, // class -> max size
    _SmallHeap,                       // LittleHeap
    _BigHeap                          // BigHeap
>;
// 4. Thread Local Interface (No locks on fast path)
using AllocatorPro = AdapterPerThread<_Buckets>;

// =========================================================================
// 6. FINAL SELECTION
// =========================================================================

// Final ANSI Wrapper
template <class Allocator>
class TheAllocator : public HL::ANSIWrapper<Allocator> {};