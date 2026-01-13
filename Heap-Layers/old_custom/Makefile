CXX = g++
CXXFLAGS = -O3 -fPIC -I.. 

global_lock:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=Global \
		-shared hl_malloc.cpp -o libhl_global.so

recycling:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=Recycling \
		-shared hl_malloc.cpp -o libhl_recycling.so

nolock:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=NoLock \
		-shared hl_malloc.cpp -o libhl_nolock.so

clean:
	rm -f *.so
