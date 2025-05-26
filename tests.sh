#!/bin/bash
set -e

# Function to run webserv, test, then shut it down
run_test() {
    local method=$1
    local url=$2
    local expected=$3
    local data_flag=$4

    echo "🚀 Starting Webserv..."
    ./Webserv &
    SERVER_PID=$!
    sleep 2  # Give time for the server to start

    RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X $method $data_flag "$url")
    if [ "$RESPONSE" != "$expected" ]; then
        echo "❌ $method $url failed: expected $expected, got $RESPONSE"
        kill -2 $SERVER_PID
        exit 1
    fi

    echo "✅ $method $url passed ($expected)."
    kill $SERVER_PID
    wait $SERVER_PID 2>/dev/null || true
    sleep 1
}

echo "🔍 Running unit tests one by one..."

run_test GET  http://127.0.0.1:2000/                          405
# run_test POST http://127.0.0.1:2000/                          200 "-d 'test=ok'"
run_test GET  http://127.0.0.1:3000/wAvc-W3P.jpeg             200
run_test GET  http://127.0.0.1:4000/test.mp3                  200
run_test GET  http://127.0.0.1:5002/index.mp4                 200
run_test GET  http://127.0.0.1:2501/upload.html               200
run_test DELETE http://127.0.0.1:2000/                        405
run_test GET  http://127.0.0.1:2501/notfound.html             404

# Separate chunked upload test
# echo "🚀 Starting Webserv for chunked POST test..."
# ./Webserv &
# SERVER_PID=$!
# sleep 2

# RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" \
#      -X POST \
#      -H "Transfer-Encoding: chunked" \
#      --data-binary @<(printf 'a\r\nx\r\n0\r\n\r\n') \
#      http://127.0.0.1:2500/)

# if [ "$RESPONSE" != "200" ]; then
#     echo "❌ Chunked POST failed: expected 200, got $RESPONSE"
#     kill $SERVER_PID
#     exit 1
# fi

# echo "✅ Chunked POST passed (200)."
# kill -2 $SERVER_PID
# wait $SERVER_PID 2>/dev/null || true

echo "🎉 All tests passed!"