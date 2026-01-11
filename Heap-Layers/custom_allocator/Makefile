CXX = g++
CXXFLAGS = -O3 -fPIC -I.. 

# typedef AllocatorEmbedded   ;
# typedef AllocatorScratchpad CurrentAllocator;
# typedef AllocatorHybridLocked     CurrentAllocator;
# typedef AllocatorHybridPerThread CurrentAllocator;
# typedef AllocatorPro        CurrentAllocator;

AllocatorEmbedded:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=AllocatorEmbedded \
		-shared custom_allocator.cpp -o libhl_AllocatorEmbedded.so

AllocatorScratchpad:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=AllocatorScratchpad \
		-shared custom_allocator.cpp -o libhl_AllocatorScratchpad.so

AllocatorHybridLocked:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=AllocatorHybridLocked \
		-shared custom_allocator.cpp -o libhl_AllocatorHybridLocked.so

AllocatorHybridPerThread:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=AllocatorHybridPerThread \
		-shared custom_allocator.cpp -o libhl_AllocatorHybridPerThread.so

AllocatorPro:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=AllocatorPro \
		-shared custom_allocator.cpp -o libhl_AllocatorPro.so

clean:
	rm -f *.so
