#!/bin/bash

echo "BEFORE:"
ls data -RA | sed 's/^/\t/'
echo ""

rm -rf data/*
rm -rf data/.versions
cp -r ../tests/default_data/* data

echo "AFTER:"
ls data -RA | sed 's/^/\t/'
echo ""