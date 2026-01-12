#!/bin/bash

libs="SimpleBuffer MmapArena HybridPerThread HybridLocked Segregated"

exe="./test"
echo

for lib in $libs
do
    echo -e "Using policy: $lib\n"
    LD_PRELOAD="./libs/libhl_$lib.so" $exe
    echo -e "------------------\n" 
done
