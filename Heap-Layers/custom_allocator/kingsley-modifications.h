#pragma once
#include "custom_policy.h"

template <int NUM_BINS, class SmallHeap, class BigHeap>
using PolicyKingsley = HL::StrictSegHeap<
    NUM_BINS, 
    Kingsley::size2Class, 
    Kingsley::class2Size, 
    SmallHeap,
    BigHeap
>;

template <class Source, size_t ChunkSize, class ListType>
using KingsleySmallHeap = LayoutAdapt<ListType, LayoutUniqueZone<Source, ChunkSize>>;

template <class Source, size_t ChunkSize>
using KingsleyBigHeap = LayoutUniqueZone<SourceMmap, 64 * 1024>;

using KingsleySmall = KingsleySmallHeap<SourceMmap, 64 * 1024, DLList>;
using KingsleyBig = KingsleyBigHeap<SourceMmap, 64 * 1024>;

using KingsleyAlloc = PolicyKingsley<29, KingsleySmall, KingsleyBig>;

using KingsleyPT = PerThread<KingsleyAlloc>;
using KingsleyLocked = LockMutex<KingsleyAlloc>;

using KingsleySmallSLL = KingsleySmallHeap<SourceMmap, 64 * 1024, SLList>;
using KingsleySLL = PolicyKingsley<29, KingsleySmallSLL, KingsleyBig>;
using KingsleySLLPT = PerThread<KingsleySLL>;
using KingsleySLLLocked = LockMutex<KingsleySLL>;

using _KingsleySmall_BigChunk = KingsleySmallHeap<SourceMmap, 256 * 1024, DLList>;
using _KingsleyBig_BigChunk = KingsleyBigHeap<SourceMmap, 256 * 1024>;
using KingsleyBigChunk = PolicyKingsley<29, _KingsleySmall_BigChunk, _KingsleyBig_BigChunk>;
using KingsleyBigChunkPT = PerThread<KingsleyBigChunk>;
using KingsleyBigChunkLocked = LockMutex<KingsleyBigChunk>;

using Kingsley25Bins = PolicyKingsley<25, KingsleySmall, KingsleyBig>;
using Kingsley25BinsPT = PerThread<Kingsley25Bins>;
using Kingsley25BinsLocked = LockMutex<Kingsley25Bins>;

using Kingsley20Bins = PolicyKingsley<20, KingsleySmall, KingsleyBig>;
using Kingsley20BinsPT = PerThread<Kingsley20Bins>;
using Kingsley20BinsLocked = LockMutex<Kingsley20Bins>;

using Kingsley16Bins = PolicyKingsley<16, KingsleySmall, KingsleyBig>;
using Kingsley16BinsPT = PerThread<Kingsley16Bins>;
using Kingsley16BinsLocked = LockMutex<Kingsley16Bins>;