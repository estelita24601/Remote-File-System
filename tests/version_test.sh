#!/bin/bash

OUT=../tests/output_version
. script_helpers.sh

cd ..
setup

# make sure client has default files
cp -r tests/default_data/* client/data

# remove previous test output files
rm -f "$OUT/test*"

cd server
print_header "Starting Server in Background"
(./rfs_server 2>&1 | tee "$OUT/server.log") &
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

cd ../client
print_header "TEST 1: write 112KB text file to same remote path twice"
run_cmd_and_save "$OUT/test_1a.txt" ./rfs WRITE --local file_112kb.txt
run_cmd_and_save "$OUT/test_1b.txt" ./rfs WRITE --local file_112kb.txt
display_server_data

print_header "TEST 2: write different content to same file 3 times"
echo "version 1" > data/v1.txt
echo "version 2" > data/v2.txt
echo "version 3" > data/v3.txt
run_cmd_and_save "$OUT/test_2a.txt" ./rfs WRITE --local v1.txt --remote test.txt
run_cmd_and_save "$OUT/test_2b.txt" ./rfs WRITE --local v2.txt --remote test.txt
run_cmd_and_save "$OUT/test_2c.txt" ./rfs WRITE --local v3.txt --remote test.txt
display_server_data

print_header "TEST 3: GET returns most recent version"
run_cmd_and_save "$OUT/test_3.txt" ./rfs GET --remote test.txt --local retrieved_test.txt
echo ""
echo "EXPECTED:"
echo "version 3"
echo ""
echo "ACTUAL:"
cat data/retrieved_test.txt
display_client_data

print_header "TEST 4: make sure deleting a file also deletes all version history"
run_cmd_and_save "$OUT/test_4.txt" ./rfs RM --remote test.txt
display_server_data

print_header "End of Tests: Killing Server"
# kill server
kill $SERVER_PROCESS_ID