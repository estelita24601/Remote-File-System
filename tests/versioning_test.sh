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
rm -rf server/data/*
rm -rf server/data/.versions

# make sure client has test files
rm -rf client/data/*
cp -r tests/data/* client/data

# remove previous client output files
rm -f tests/output/version*

cd server
echo ""
echo "======== Starting Server in Background ========"
./rfs_server &
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

cd ../client
echo ""
echo "======== TEST 1: write 112KB text file to same remote path twice ========"
echo "./rfs WRITE --local file_112kb.txt"
./rfs WRITE --local file_112kb.txt &> ../tests/output/version_test_1a.txt

echo "./rfs WRITE --local file_112kb.txt"
./rfs WRITE --local file_112kb.txt &> ../tests/output/version_test_1b.txt

echo ""
echo "Checking for version files..."
ls ../server/data/.versions/

echo ""
echo "======== TEST 2: write different content to same file 3 times ========"

echo "version 1" > data/v1.txt
echo "./rfs WRITE --local v1.txt --remote test.txt"
./rfs WRITE --local v1.txt --remote test.txt

echo "version 2" > data/v2.txt
echo "./rfs WRITE --local v2.txt --remote test.txt"
./rfs WRITE --local v2.txt --remote test.txt

echo "version 3" > data/v3.txt
echo "./rfs WRITE --local v3.txt --remote test.txt"
./rfs WRITE --local v3.txt --remote test.txt

echo ""
echo "Checking for version files..."
ls ../server/data/.versions/


echo ""
echo "======== End of Tests: Killing Server ========"
# kill server
kill $SERVER_PROCESS_ID