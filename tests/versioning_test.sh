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
rm -f tests/output/version_*

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
./rfs WRITE --local file_112kb.txt &> ../tests/output/versiontest_1a.txt

echo "./rfs WRITE --local file_112kb.txt"
./rfs WRITE --local file_112kb.txt &> ../tests/output/version_test_1b.txt


echo ""
echo "======== End of Tests: Killing Server ========"
# kill server
kill $SERVER_PROCESS_ID