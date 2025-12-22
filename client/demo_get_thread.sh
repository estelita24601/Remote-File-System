#!/bin/bash

./rfs WRITE --local small_picture.png
sleep 5

for i in {1..5}; do
    ./rfs GET --remote small_picture.png --local thread_TEST_$i.png &
done
