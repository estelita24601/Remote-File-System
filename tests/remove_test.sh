#!/bin/bash

. script_helpers.sh
OUT=../tests/output_remove

cd ..
setup

# remove previous test output files
rm -f tests/output_remove/*
# copy default files to the server
cp -r tests/default_data/* server/data
# but going to delete a few files so its easier to tell when things are deleted
rm -f server/data/large_picture.jpg server/data/small_picture.png server/data/test.csv

cd server
print_header "Starting Server in Background"
(./rfs_server 2>&1 | tee "$OUT/server.log") &
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

print_header "BEFORE ANY TESTS"
display_server_data

cd ../client
print_header "TEST 1: try to delete non-existent file"
run_cmd_and_save "$OUT/test_1.txt" ./rfs RM --remote doesnt_exist.txt
display_server_data

print_header "TEST 2: delete existing file"
run_cmd_and_save "$OUT/test_2.txt" ./rfs RM --remote file_112kb.txt
display_server_data

print_header "TEST 3: delete file in nested folder"
run_cmd_and_save "$OUT/test_3.txt" ./rfs RM --remote my_folder/hello.txt
display_server_data

print_header "TEST 4: delete empty folder"
run_cmd_and_save "$OUT/test_4.txt" ./rfs RM --remote my_folder
display_server_data

print_header "End of Tests: Killing Server"
kill $SERVER_PROCESS_ID
