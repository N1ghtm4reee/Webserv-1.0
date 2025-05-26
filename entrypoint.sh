#!/bin/bash

if [ "$1" = "test" ]; then
    ./tests.sh
elif [ "$1" = "dast" ]; then
    ./dast.sh
else
    echo "Usage: $0 [test|dast]"
    exit 1
fi
