#!/bin/bash

echo "10 clients writing different local files to the same remote file"
for i in {1..10}; do
    ./rfs WRITE --server 34.94.110.163 --local "v$i.txt" --remote thread_test.txt&
done