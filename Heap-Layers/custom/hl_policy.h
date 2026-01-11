#pragma once
#include <mutex>
#include "heaplayers.h"

// ---- Base building blocks ----
using BaseHeap = HL::StaticBufferHeap<1024 * 1024>; // 1 MB buffer

// Locking
using Locked = HL::LockedHeap<std::recursive_mutex, BaseHeap>;
using NoLock  = BaseHeap;

// Threading
using PerThread = HL::ThreadSpecificHeap<Locked>;
using Global    = Locked;

// Object reuse policy
using Recycling = HL::FreelistHeap<PerThread>;
using NoReuse   = PerThread;

// Final ANSI interface
template <class Heap>
using Allocator = HL::ANSIWrapper<Heap>;
