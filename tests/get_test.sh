#!/bin/bash

cd ..
echo "======== Building Project ========="
make clean
make || exit 1

echo "======== Cleaning up old processes ========"
killall -9 rfs_server
sleep 3

echo ""
echo "======== Removing Old Files ========"
# make client empty so we can tell what was written during this test
rm -f client/data/*

# make sure server has test files
rm -f server/data/*
cp -r tests/data/* server/data

# remove previous client output files
rm -f tests/output/get_*

cd server
echo ""
echo "======== Starting Server in Background ========"
./rfs_server &
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

cd ../client
echo ""
echo "======== TEST 1: request the 112KB text file from the server with default local path ========"
echo "./rfs GET --remote file_112kb.txt"
./rfs GET --remote file_112kb.txt &> ../tests/output/get_test_1.txt

# todo: test 2 request file with custom local path

# todo: test 3 request different type of file

echo ""
echo "======== TEST 4: multiple clients  ========"
for i in {1..10}; do
    echo "./rfs GET --remote file_112kb.txt --local thread_test_$i.txt"
    ./rfs GET --remote file_112kb.txt --local thread_test_$i.txt &> ../tests/output/get_test_thread_$i.txt &
done
sleep 5 # wait for all threads to finish

echo ""
echo "======== End of Tests: Killing Server ========"
# kill server
kill $SERVER_PROCESS_ID