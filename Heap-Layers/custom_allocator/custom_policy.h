#pragma once
#include "heaplayers.h"
#include <mutex>
#include "malign.h"

// 1. RAW SOURCES (Where bytes come from)

// Infinite Memory (The "City Water Main")
using SourceMmap = HL::MmapHeap; 

// Fixed Memory (The "Bucket")
template <size_t Size = 1024 * 1024> // 1 MB default
using SourceStatic = HL::StaticBufferHeap<Size>;

// Standard System malloc/free
using SourceSysMalloc = HL::MallocHeap;

// 2. LAYOUTS (How we organize bytes)
// Strategy A: The "Cookie Cutter" (Zone) - Fast, No free()
template <class Source, int ChunkSize = 64 * 1024>
using LayoutZone = HL::SizeHeap<HL::ZoneHeap<Source, ChunkSize>>;

// Strategy B: The "Organizer" (Freelist) - Standard malloc
template <class Source>
using LayoutFreelist = HL::SizeHeap<HL::FreelistHeap<Source>>;

template <class ListType, class Source>
using LayoutAdapt = HL::AdaptHeap<ListType, Source>;

// 3. POLICIES (The Traffic Cops)

// Router A: The "Hybrid Splitter" (SizeHeap)
// Routes small allocs to 'SmallHeap', large allocs to 'BigHeap'.
// Threshold is usually 256 or 1024 bytes.
template <size_t size, class SmallHeap, class BigHeap>
using PolicySplitter = HL::HybridHeap<size, SmallHeap, BigHeap>;

// Router B: The "Perfect Fit" (SegregatedHeap)
// Creates an array of heaps (one for size 8, one for 16, etc).
// This is used for high-performance "Bucket" allocators.

template <int NUM_BINS, class SmallHeap, class BigHeap>
using PolicySeg = HL::SegHeap<
    NUM_BINS, 
    Kingsley::size2Class, 
    Kingsley::class2Size, 
    SmallHeap, 
    BigHeap
>;

template <int NUM_BINS, class SmallHeap, class BigHeap>
using PolicyStrictSeg = HL::StrictSegHeap<
    NUM_BINS,
    Kingsley::size2Class,
    Kingsley::class2Size,
    SmallHeap,
    BigHeap
>;

// 4. ADAPTERS (Concurrency & Safety)

// Thread Safety: Global Lock
template <class Heap>
using LockMutex = HL::LockedHeap<std::recursive_mutex, Heap>;

// Thread Safety: Per-Thread Cache
template <class Heap>
using PerThread = HL::ThreadSpecificHeap<Heap>;

// 5. PRE-BUILT RECIPES

// RECIPE 1: The "Embedded" (Simple)
//using SimpleBuffer = LockMutex<LayoutFreelist<SourceStatic<1024*1024>>>;
using SimpleBuffer = LockMutex<LayoutFreelist<Malign<16, SourceStatic<64 * 1024 * 1024>>>>;
using SimpleMmap = LockMutex<LayoutFreelist<SourceMmap>>;

using SimpleSys = LockMutex<LayoutFreelist<SourceSysMalloc>>;

// RECIPE 2: The "Scratchpad" (Fastest)
using MmapArena = PerThread<LayoutZone<SourceMmap>>;

// RECIPE 3: The "Smart Hybrid" (SizeHeap)
// Scenario: We want fast Zones for small items, but we don't want to 
// waste memory for large items.
// 1. Define the two paths
// using _SmallPath = LayoutZone<SourceMmap>;      // Fast!
// using _BigPath   = LayoutFreelist<SourceMmap>;  // Smart!
// // 2. Combine them (Split at 256 bytes)
// using _Hybrid    = PolicySplitter<256, _SmallPath, _BigPath>;
// // 3. Lock it
// using HybridLocked = LockMutex<_Hybrid>;
// using HybridPerThread = PerThread<_Hybrid>;


// A. Small Objects (Fast Path)
//    - Uses a Zone (Bump Pointer) for speed.
//    - ZoneHeap automatically checks "contains(ptr)" so we know if a ptr belongs here.
// using SegSmallHeap = HL::AdaptHeap<DLList, HL::SizeHeap<HL::ZoneHeap<SourceMmap, 4096>>>;
// using SegBigPath = HL::SizeHeap<HL::UniqueHeap<HL::ZoneHeap<SourceMmap, 65536>>>;
// using StrictSeg = HL::LockedHeap<std::recursive_mutex, HL::StrictSegHeap< 
// 29,                               // NumBins
// Kingsley::size2Class,             // size -> class
// Kingsley::class2Size,             // class -> max Size
// SegSmallHeap,                     // PerClassHeap
// SegBigPath                        // BigHeap
// >>;

using SegSmallHeap = LayoutAdapt<DLList, LayoutZone<SourceMmap, 4096>>;
using SegBigHeap = LayoutZone<SourceMmap, 65536>;

using Segregated = LockMutex<PolicySeg<29, SegSmallHeap, SegBigHeap>>;
using StrictSeg = LockMutex<PolicyStrictSeg<29, SegSmallHeap, SegBigHeap>>;

// using _SmallPath = HL::FreelistHeap<SourceMmap>;
// B. Big Objects (Slow Path)
//    - Uses direct mmap for large objects.
//    - No FreelistHeap (to avoid header collisions).
using HybridSmallPath = HL::FreelistHeap<Malign<16, LayoutZone<SourceMmap, 4096>>>;
using HybridBigPath = HL::SizeHeap<SourceMmap>;
using _Hybrid = Malign<16, HL::HybridHeap<8192, HybridSmallPath, HybridBigPath>>;
using Hybrid = HL::LockedHeap<std::recursive_mutex, _Hybrid>;

// RECIPE 4: The "Pro Tier" (Segregated + PerThread)
// This mimics Mimalloc / TCMalloc architecture.
// 1. The Global Source (Locked once per 64KB chunk)
using _Global = LockMutex<SourceMmap>;
// 2. The Page Factory (Zones that pull from Global)
using _SmallHeap   = LayoutZone<_Global>;
using _BigHeap     = LayoutFreelist<_Global>;
// 3. The Buckets (Array of Pages)
using _Buckets = PolicySeg<
    256,                              // NumBins
    _SmallHeap,                       // LittleHeap
    _BigHeap                          // BigHeap
>;
// 4. Thread Local Interface (No locks on fast path)
using SegregatedPerThread = PerThread<HL::SizeHeap<_Buckets>>;

// Final ANSI Wrapper
template <class Allocator>
class TheAllocator : public HL::ANSIWrapper<Allocator> {};
