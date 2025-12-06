#!/bin/bash

. script_helpers.sh

cd ..
setup
# copy default files to the server
cp -r tests/data/* server/data
# remove previous test output files
rm -f tests/output_get/*

cd server
print_header "Starting Server in Background"
(./rfs_server 2>&1 | tee ../tests/output_get/server.log) &
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

cd ../client

print_header "TEST 1: request the 112KB text file from the server with default local path"
run_cmd_and_save ../tests/output_get/test_1.txt ./rfs GET --remote file_112kb.txt
display_client_data

print_header "TEST 2: request the 112KB text file from the server with a custom local path"
run_cmd_and_save ../tests/output_get/test_2.txt ./rfs GET --remote file_112kb.txt --local custom_local_112kb.txt
display_client_data

print_header "TEST 3: request the csv file from the server with a custom local path"
run_cmd_and_save ../tests/output_get/test_3.text ./rfs GET --remote test.csv --local custom_local_csv.txt
display_client_data

print_header "TEST 4: multiple clients"
for i in {1..10}; do
    run_cmd_and_save ../tests/output_get/test_thread_$i.txt ./rfs GET --remote file_112kb.txt --local thread_test_$i.txt &
done
sleep 5 # wait for all threads to finish
display_client_data

print_header "End of Tests: Killing Server"
# kill server
kill $SERVER_PROCESS_ID