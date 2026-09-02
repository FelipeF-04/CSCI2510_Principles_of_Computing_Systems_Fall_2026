#!/bin/bash

echo "LINES:" $(wc -l < $1)

echo "STATUS_COUNTS:"
awk '{print $2}' $1 | sort |uniq -c | sort -k 1 -r | awk '{print $2, $1}'

