#!/bin/bash

libs="SimpleBuffer SimpleMmap MmapArena Hybrid Segregated StrictSeg SegregatedPerThread" #HybridLocked HybridPerThread

exe="./test"
echo

for lib in $libs
do
    echo -e "Using policy: $lib\n"
    LD_PRELOAD="./libs/hl_$lib.so" $exe
    echo -e "------------------\n" 
done
