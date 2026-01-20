CXX = g++
CXXFLAGS = -g -O3 -fPIC -I..
TARGET_DIR = libs

# 1. Define the list of Policy names
# POLICIES = SimpleBuffer SimpleMmap SimpleSys MmapArena Segregated StrictSeg SegregatedPerThread SegregatedPerThreadPlus SegPTFreelist StrictSegregatedPerThreadFree StrictSegregatedPerThreadNoFree#Hybrid HybridPerThread HybridLocked HybridSegregated StrictSegregatedPerThread 
#POLICIES =  KingsleyAlloc KingsleyPT KingsleyLocked \
			KingsleySLL KingsleySLLLocked KingsleySLLPT \
			KingsleyBigChunk KingsleyBigChunkPT KingsleyBigChunkLocked \
			Kingsley25Bins Kingsley25BinsPT Kingsley25BinsLocked \
			Kingsley20Bins Kingsley20BinsPT Kingsley20BinsLocked \
			Kingsley16Bins Kingsley16BinsPT Kingsley16BinsLocked
POLICIES = StrictSegregatedPTNoFreeCustom StrictSegregatedLockedNoFreeCustom StrictSegregatedPTNoFree StrictSegregatedLockedNoFree SegregatedPerThread

# 2. Generate the list of expected library files
TARGET_LIBS = $(patsubst %, $(TARGET_DIR)/hl_%.so, $(POLICIES))

# Mark the policy names as phony targets (so they work even if a file named 'SimpleBuffer' exists)
.PHONY: all clean rebuild $(POLICIES)

all: $(TARGET_LIBS) test

$(TARGET_LIBS): $(TARGET_DIR)/hl_%.so : custom_allocator.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) -DHL_POLICY=$* -shared $< -o $@

$(POLICIES): % : $(TARGET_DIR)/hl_%.so
	@echo "Build complete for policy: $@"

test: test.c
	gcc -g -O0 -pthread test.c -o test

clean:
	rm -f $(TARGET_DIR)/*.so test

rebuild: clean all
