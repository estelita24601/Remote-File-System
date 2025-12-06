#!/bin/bash

OUT=../tests/output_get
. script_helpers.sh

cd ..
setup
# copy default files to the server
cp -r tests/default_data/* server/data
# remove previous test output files
rm -f tests/output_get/*

cd server
print_header "Starting Server in Background"
(./rfs_server 2>&1 | tee "$OUT/server.log") &
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

cd ../client

print_header "TEST 1: try to request non-existant file from the server"
run_cmd_and_save "$OUT/test_1.txt" ./rfs GET --remote doesnt_exist.txt
display_client_data

print_header "TEST 2: request the 112KB text file from the server with default local path"
run_cmd_and_save "$OUT/test_2.txt" ./rfs GET --remote file_112kb.txt
display_client_data

print_header "TEST 3: request the 112KB text file from the server with a custom local path"
run_cmd_and_save "$OUT/test_3.txt" ./rfs GET --remote file_112kb.txt --local custom_local_112kb.txt
display_client_data

print_header "TEST 4: request the csv file from the server with a custom local path"
run_cmd_and_save "$OUT/test_4.txt" ./rfs GET --remote test.csv --local custom_local_csv.txt
display_client_data

print_header "TEST 5: request picture from the server"
run_cmd_and_save "$OUT/test_5.txt" ./rfs GET --remote small_picture.png
display_client_data

print_header "TEST 6: request a file from the server with multiple layers of folders"
run_cmd_and_save "$OUT/test_6.txt" ./rfs GET --remote my_folder/hello.txt
display_client_data

print_header "TEST 7: multiple clients ask for the same file"
for i in {1..10}; do
    run_cmd_and_save "$OUT/test_thread_$i.txt" ./rfs GET --remote file_112kb.txt --local thread_test_$i.txt &
done
sleep 5 # wait for all threads to finish
display_client_data



print_header "End of Tests: Killing Server"
# kill server
kill $SERVER_PROCESS_ID