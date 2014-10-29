#!/bin/bash
file=$(echo "$1_$2" | sed "s/,/_/g")
cat "$1.log" | sed -n  "1p;/$2,/p" | tee "../$file.log"
echo "Written to: " $file
