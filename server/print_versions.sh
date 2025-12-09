#!/bin/bash

echo "==== thread_test.txt"
cat thread_test.txt
echo

for f in .versions/thread_test.txt.*; do
    echo "===== $f"
    cat "$f"
    echo
done
