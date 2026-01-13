#!/bin/bash

exe="./test"
echo

# Iterate over all .so files in libs/ directory
for lib_path in ./libs/*.so
do
    # Extract policy name from filename (e.g., hl_SegregatedPerThread.so -> SegregatedPerThread)
    lib_name=$(basename "$lib_path" .so | sed 's/hl_//')
    
    echo -e "Using policy: $lib_name\n"
    LD_PRELOAD="$lib_path" $exe
    echo -e "------------------\n" 
done
