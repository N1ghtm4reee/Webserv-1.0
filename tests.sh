#!/bin/bash

set -e

# Start webserv in the background
./Webserv &
SERVER_PID=$!

# Give it time to boot
sleep 1

# test 1 GET
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" http://127.0.0.1:2000/)
if [ "$RESPONSE" != "200" ]; then
  echo "GET / failed with status $RESPONSE"
  exit 1
fi
echo "GET / passed."

# RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://127.0.0.1:2500/ -d "name=webserv")
# if [ "$RESPONSE" != "200" ]; then
#   echo "POST / failed with status $RESPONSE"
#   exit 1
# fi
# echo "POST / passed."

# # Test 3: 404
# RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/notfound)
# if [ "$RESPONSE" != "404" ]; then
#   echo "404 check failed with status $RESPONSE"
#   kill $SERVER_PID
#   exit 1
# fi
# echo "404 passed."

# # Kill server after test
# kill $SERVER_PID
# echo "All tests passed."
