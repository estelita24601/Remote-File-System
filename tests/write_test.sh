#!/bin/bash
OUT=../tests/output_write
. script_helpers.sh

cd ..
setup

# remove previous test output files
rm -f tests/output_write/*
# give client the default files
cp -r tests/default_data/* client/data

cd server
print_header "Starting Server in Background"
# redirect stdout and stderr to logfile and still print it out
(./rfs_server 2>&1 | tee "$OUT/server.log" )&
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

cd ../client

print_header "TEST 1: try to send non-existent file to the server"
run_cmd_and_save "$OUT/test_1.txt" ./rfs WRITE --local doesnt_exist.txt --remote write1.txt
display_server_data

print_header "TEST 2: write 112KB text file with default remote path"
run_cmd_and_save "$OUT/test_2.txt" ./rfs WRITE --local file_112kb.txt
display_server_data

print_header "TEST 3: write 112KB text file with custom remote path"
run_cmd_and_save "$OUT/test_3.txt" ./rfs WRITE --local file_112kb.txt --remote test2_result.txt
display_server_data

print_header "TEST 4: write text containing ASCII art to the server with custom remote path"
run_cmd_and_save "$OUT/test_4.txt" ./rfs WRITE --local ascii_art.txt --remote test3_result.txt
display_server_data

print_header "TEST 5: send a picture to the server"
run_cmd_and_save "$OUT/test_5.txt" ./rfs WRITE --local small_picture.png
display_server_data

print_header "TEST 6: send file to server and try to put it into nested folder"
run_cmd_and_save "$OUT/test_6.txt" ./rfs WRITE --local hello.txt --remote subfolder/hello.txt
display_server_data

print_header "TEST 7: send file to server with multiple layers of folders"
run_cmd_and_save "$OUT/test_7.txt" ./rfs WRITE --local hello.txt --remote subfolder/another_folder/third_layer/hello.txt
display_server_data

print_header "TEST 8: multiple clients writing the same content to different filenames"
for i in {1..10}; do
    run_cmd_and_save "$OUT/test_thread_$i.txt" ./rfs WRITE --local file_112kb.txt --remote thread_test_$i.txt &
done
sleep 5 # wait for all threads to finish
display_server_data


print_header "End of Tests: Killing Server"
# kill server
kill $SERVER_PROCESS_ID