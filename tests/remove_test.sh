#!/bin/bash

. script_helpers.sh

cd ..
setup

# remove previous test output files
rm -f tests/output_remove/*
# copy default files to the server
cp -r tests/data/* server/data

cd server
print_header "Starting Server in Background"
(./rfs_server 2>&1 | tee ../tests/output_get/server.log) &
SERVER_PROCESS_ID=$! # save for later
sleep 1 # give server enough time to start up

cd ../client

# TODO