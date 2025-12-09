#!/bin/bash

echo "BEFORE:"
ls data -RA | sed 's/^/\t/'
echo ""

rm -rf data/*
rm -rf data/.versions

echo "AFTER:"
ls data -RA | sed 's/^/\t/'
echo ""