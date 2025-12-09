#!/bin/bash

echo "BEFORE:"
ls data -RA | sed 's/^/\t/'
echo ""

rm -rf data/*

echo "AFTER:"
ls data -RA | sed 's/^/\t/'
echo ""