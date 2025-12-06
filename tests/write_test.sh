#!/bin/bash

. script_helpers.sh

cd ..
setup

# remove previous test output files
rm -f tests/output_write/*
# give client the default files
cp -r tests/data/* client/data

cd server
print_header "Starting Server in Background"
# redirect stdout and stderr to logfile and still print it out
(./rfs_server 2>&1 | tee ../tests/output_write/server.log )&
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

cd ../client
print_header "TEST 1: write 112KB text file with default remote path"
run_cmd_and_save ../tests/output_write/test_1.txt ./rfs WRITE --local file_112kb.txt
display_server_data

print_header "TEST 2: write 112KB text file with custom remote path"
run_cmd_and_save ../tests/output_write/test_2.txt ./rfs WRITE --local file_112kb.txt --remote test2_result.txt
display_server_data

print_header "TEST 3: write text containing ASCII art to the server with custom remote path"
run_cmd_and_save ../tests/output_write/test_3.txt ./rfs WRITE --local ascii_art.txt --remote test3_result.txt
display_server_data

print_header "TEST 4: multiple clients writing the same content to different filenames"
for i in {1..10}; do
    run_cmd_and_save ../tests/output_write/test_thread_$i.txt ./rfs WRITE --local file_112kb.txt --remote thread_test_$i.txt &
done
sleep 5 # wait for all threads to finish
display_server_data

print_header "End of Tests: Killing Server"
# kill server
kill $SERVER_PROCESS_ID