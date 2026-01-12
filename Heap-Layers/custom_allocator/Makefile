CXX = g++
CXXFLAGS = -O3 -fPIC -I.. 
TARGET_DIR= libs
all: SimpleBuffer SimpleMmap MmapArena HybridLocked HybridPerThread Segregated

SimpleBuffer:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=SimpleBuffer \
		-shared custom_allocator.cpp -o $(TARGET_DIR)/libhl_SimpleBuffer.so

SimpleMmap:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=SimpleMmap \
		-shared custom_allocator.cpp -o $(TARGET_DIR)/libhl_SimpleMmap.so

MmapArena:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=MmapArena \
		-shared custom_allocator.cpp -o $(TARGET_DIR)/libhl_MmapArena.so

HybridLocked:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=HybridLocked \
		-shared custom_allocator.cpp -o $(TARGET_DIR)/libhl_HybridLocked.so

HybridPerThread:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=HybridPerThread \
		-shared custom_allocator.cpp -o $(TARGET_DIR)/libhl_HybridPerThread.so

Segregated:
	$(CXX) $(CXXFLAGS) -DHL_POLICY=Segregated \
		-shared custom_allocator.cpp -o $(TARGET_DIR)/libhl_Segregated.so

clean:
	rm -f $(TARGET_DIR)/*.so
