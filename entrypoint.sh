#!/bin/bash

if [ "$1" = "test" ]; then
    /home/Webserv/tests.sh
elif [ "$1" = "dast" ]; then
    ./Webserv
else
    echo "Usage: $0 [test|dast]"
    exit 1
fi
