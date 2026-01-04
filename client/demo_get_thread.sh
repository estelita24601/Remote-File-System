#!/bin/bash

FILE="small_picture.png"

./rfs WRITE --local $FILE
sleep 5

for i in {1..5}; do
    ./rfs GET --remote $FILE --local thread_TEST_$i.png &
done
