CXX = g++
CXXFLAGS = -g -O3 -fPIC -I..
TARGET_DIR = libs

# 1. Define the list of Policy names
POLICIES = SimpleBuffer SimpleMmap SimpleSys MmapArena Segregated StrictSeg SegregatedPerThread SegregatedPerThreadPlus HybridSegregated StrictSegregatedPerThread HybridPerThread HybridLocked #Hybrid 

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
