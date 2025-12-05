#!/bin/bash

cd ..
echo "======== Building Project ========="
make clean
make || exit 1

echo "======== Cleaning up old processes ========"
pkill -9 rfs_server
sleep 1

echo ""
echo "======== Reseting Files ========"
# make server empty so we can tell what was written during this test
rm -f -r server/data/*

# make sure client has test files
rm -f -r client/data/*
cp -r tests/data/* client/data

# remove previous client output files
rm -f tests/output/write_*

cd server
echo ""
echo "======== Starting Server in Background ========"
./rfs_server &
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

cd ../client
echo ""
echo "======== TEST 1: write 112KB text file with default remote path ========"
echo "./rfs WRITE --local file_112kb.txt"
./rfs WRITE --local file_112kb.txt  &> ../tests/output/write_test_1.txt

echo ""
echo "======== TEST 2: write 112KB text file with custom remote path ========"
echo "./rfs WRITE --local file_112kb.txt --remote test2_result.txt"
./rfs WRITE --local file_112kb.txt --remote test2_result.txt &> ../tests/output/write_test_2.txt

echo ""
echo "======== TEST 3: write text containing ASCII art to the server with custom remote path ========"
echo "./rfs WRITE --local ascii_art.txt --remote test3_result.txt"
./rfs WRITE --local ascii_art.txt --remote test3_result.txt &> ../tests/output/write_test_3.txt

echo ""
echo "======== TEST 4: multiple clients writing the same content to different filenames ========"
for i in {1..10}; do
    echo "./rfs WRITE --local file_112kb.txt --remote thread_test_$i.txt"
    ./rfs WRITE --local file_112kb.txt --remote thread_test_$i.txt &> ../tests/output/write_test_thread_$i.txt &
done
sleep 5 # wait for all threads to finish

echo ""
echo "======== End of Tests: Killing Server ========"
# kill server
kill $SERVER_PROCESS_ID