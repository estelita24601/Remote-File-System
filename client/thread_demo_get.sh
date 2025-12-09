#!/bin/bash

echo "10 clients trying to get the same remote file into different local files"
for i in {1..10}; do
    ./rfs GET --server 34.94.110.163 --remote hello.txt --local thread_test_$i.txt &
done