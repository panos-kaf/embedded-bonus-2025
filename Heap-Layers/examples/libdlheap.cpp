#include <stdlib.h>

volatile int anyThreadCreated = 1;

#include "heaplayers.h"

// Define the upstream source (Your existing ZoneHeap setup)
class TopHeap : public SizeHeap<UniqueHeap<ZoneHeap<MmapHeap, 65536> > > {};

// Define a "Fixed" LeaHeap
// The original LeaHeap wraps the SmallHeap in a "Threshold" layer. 
// We are removing "Threshold" because it causes the freeAll error.
template <class Sbrk, class Mmap>
class FixedLeaHeap :
  public
    SelectMmapHeap<128 * 1024,
      // REMOVED: Threshold<4096, ... >
      DLSmallHeapType<DLBigHeapType<CoalesceableHeap<Sbrk> > >,
      CoalesceableMmapHeap<Mmap> >
{};

// Instantiate the Fixed Heap
class TheCustomHeapType :
  public ANSIWrapper< LeaHeap<TopHeap, MmapHeap> > {};

inline static TheCustomHeapType * getCustomHeap() {
  static char thBuf[sizeof(TheCustomHeapType)];
  static TheCustomHeapType * th = new (thBuf) TheCustomHeapType;
  return th;
}

#if defined(_WIN32)
#pragma warning(disable:4273)
#endif

#include "printf.h"

#if !defined(_WIN32)
#include <unistd.h>

extern "C" {
  void _putchar(char ch) { ::write(1, (void *)&ch, 1); }
}
#endif

#include "wrappers/generic-memalign.cpp"
#include "wrappers/wrapper.cpp" 

extern "C" {
  
  void * xxmalloc (size_t sz) {
    //const char msg[] = "Hello from custom malloc\n";
    //write(1, msg, sizeof(msg) - 1);
    auto ptr = getCustomHeap()->malloc (sz);
    return ptr;
  }

  void xxfree (void * ptr) {
    getCustomHeap()->free (ptr);
  }

  void * xxmemalign(size_t alignment, size_t sz) {
    return generic_xxmemalign(alignment, sz);
  }
  
  size_t xxmalloc_usable_size (void * ptr) {
    return getCustomHeap()->getSize (ptr);
  }

  void xxmalloc_lock() {
    // getCustomHeap()->lock();
  }

  void xxmalloc_unlock() {
    // getCustomHeap()->unlock();
  }
}
