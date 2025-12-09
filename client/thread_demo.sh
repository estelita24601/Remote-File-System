#!/bin/bash

echo "10 clients writing different local files to the same remote file"
for i in {1..10}; do
    ./rfs WRITE --local "v$i.txt" --remote thread_test.txt&
done


echo "10 clients trying to get the same remote file into different local files"
for i in {1..10}; do
    ./rfs GET --remote file_112kb.txt --local thread_test_$i.txt &
done