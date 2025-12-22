#!/bin/bash

for i in {1..5}; do
    ./rfs WRITE --local v$i.txt --remote thread_TEST_$i.txt &
done
