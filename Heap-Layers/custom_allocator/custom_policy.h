#pragma once
#include <mutex>
#include "heaplayers.h"
#include "malign.h" 
#include "segregated_policy_functions.h"

// 1. RAW SOURCES (Where bytes come from)

using SourceMmap = HL::MmapHeap; 

template <size_t Size = 1024 * 1024> // 1 MB default
using SourceStatic = HL::StaticBufferHeap<Size>;

// Standard System malloc/free
using SourceSysMalloc = HL::MallocHeap;

// 2. LAYOUTS (How we organize bytes)

template <class Source, int ChunkSize = 64 * 1024>
using LayoutZone = HL::SizeHeap<HL::ZoneHeap<Source, ChunkSize>>;

template <class Source>
using LayoutFreelist = HL::SizeHeap<HL::FreelistHeap<Source>>;

template <class ListType, class Source>
using LayoutAdapt = HL::AdaptHeap<ListType, Source>;

template <class Source, size_t ChunkSize>
using LayoutUniqueZone = HL::SizeHeap<HL::UniqueHeap<HL::ZoneHeap<Source, ChunkSize>>>;

// 3. POLICIES 

// Routes small allocs to 'SmallHeap', large allocs to 'BigHeap'.
// Threshold is usually 256 or 1024 bytes.
template <size_t size, class SmallHeap, class BigHeap>
using PolicySplitter = HL::HybridHeap<size, SmallHeap, BigHeap>;

// Creates an array of heaps (one for size 8, one for 16, etc).
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

template <int NUM_BINS, class SmallHeap, class BigHeap>
using PolicyCustom = HL::StrictSegHeap<
    NUM_BINS, 
    size_to_class, 
    class_to_size, 
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


// PRE-BUILT ALLOCATORS

// Simple Allocators
using SimpleBuffer = LockMutex<LayoutFreelist<Malign<16, SourceStatic<64 * 1024 * 1024>>>>;

using SimpleMmap = LockMutex<LayoutFreelist<SourceMmap>>;

using SimpleSys = LockMutex<LayoutFreelist<SourceSysMalloc>>;

// The "Scratchpad"
using MmapArena = PerThread<LayoutZone<SourceMmap>>;


// We want fast Zones for small items, but we don't want to waste memory for large items.
// (doesnt work)

// Define the two heaps
using HybridSmallPath = HL::FreelistHeap<Malign<16, LayoutZone<SourceMmap, 4096>>>;
using HybridBigPath = HL::SizeHeap<SourceMmap>;
// Combine them 
using _Hybrid = Malign<16, HL::HybridHeap<8192, HybridSmallPath, HybridBigPath>>;
// Lock it
using Hybrid = HL::LockedHeap<std::recursive_mutex, _Hybrid>;


// Segregated Heap
using SegSmallHeap = LayoutAdapt<DLList, LayoutZone<SourceMmap, 4096>>;
using SegBigHeap = LayoutZone<SourceMmap, 65536>;

using LockedSeg = LockMutex<PolicySeg<29, SegSmallHeap, SegBigHeap>>;
using LockedStrictSeg = LockMutex<PolicyStrictSeg<29, SegSmallHeap, SegBigHeap>>;

// Segregated + PerThread
using _Global = SourceMmap;

using _SmallHeap   = LayoutZone<_Global, 64 * 1024>;   // 64KB
using _BigHeap     = LayoutZone<_Global, 256 * 1024>;

using _Buckets = PolicySeg<
    16,                               // Reduced from 32 to 16 bins
    _SmallHeap,
    _BigHeap                 
>;

using SegPT = PerThread<_Buckets>;

// Small heap now has a freelist
using FreeSmall = LayoutFreelist<_SmallHeap>;
using SegPTFreelist = PerThread<HL::SizeHeap<PolicySeg<16, FreeSmall, _BigHeap>>>;

// Increase num of bins
using _SmallHeap4KB = LayoutZone<_Global, 4096>;   // Even smaller chunks
using _Buckets64Bins = PolicySeg<
    64,                               // More bins
    _SmallHeap4KB,
    _BigHeap
>;
using SegPT64Bins = PerThread<_Buckets64Bins>;

// Strict Segregated + PerThread (Lower fragmentation)
using _StrictBuckets = PolicyStrictSeg<16, LayoutFreelist<LayoutZone<_Global, 64 * 1024>>, LayoutFreelist<_Global>>;
using _StrictBucketsNoFree = PolicyStrictSeg<16, LayoutFreelist<LayoutZone<_Global, 64 * 1024>>, _Global>;

using StrictSegPT = PerThread<_StrictBuckets>;
using StrictSegPTNoFree = PerThread<_StrictBucketsNoFree>;

using StrictSegPTCustom = PolicyCustom<32, LayoutFreelist<LayoutZone<_Global, 64 * 1024>>, LayoutFreelist<_Global>>;
using StrictSegPTNoFreeCustom = PolicyCustom<32, LayoutFreelist<LayoutZone<_Global, 64 * 1024>>, _Global>;

// Final ANSI Wrapper
template <class Allocator>
class TheAllocator : public HL::ANSIWrapper<Allocator> {};