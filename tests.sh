#!/bin/bash
set -e

# Start webserv in the background
./Webserv &
SERVER_PID=$!
sleep 2

function assert_status {
    local method=$1
    local url=$2
    local expected=$3
    local data_flag=$4

    RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X $method $data_flag "$url")
    if [ "$RESPONSE" != "$expected" ]; then
        echo "$method $url failed: expected $expected, got $RESPONSE"
        kill $SERVER_PID
        exit 1
    fi
    echo "$method $url passed ($expected)."
}

echo "🔍 Running unit tests..."

# 1. GET root (should be 405 since only POST allowed)
assert_status GET http://127.0.0.1:2000/ 405
sleep 2

# 2. POST root (allowed)
# assert_status POST http://127.0.0.1:2000/ 200 "-d 'test=ok'"

# 3. GET image (jpeg)
# assert_status GET http://127.0.0.1:3000/wAvc-W3P.jpeg 200

# # 4. GET MP3
# assert_status GET http://127.0.0.1:4000/test.mp3 200
# sleep 2

# # 5. GET MP4
# assert_status GET http://127.0.0.1:5002/index.mp4 200
# sleep 2

# # 6. GET upload.html
# assert_status GET http://127.0.0.1:2501/upload.html 200
# sleep 2

# # 7. DELETE on POST-only server (expect 405)
# assert_status DELETE http://127.0.0.1:2000/ 405
# sleep 2

# # 8. 404 on missing file
# assert_status GET http://127.0.0.1:2501/notfound.html 404
# sleep 2

# 9. Chunked Upload (simulate chunked POST)
# curl -s -o /dev/null -w "%{http_code}" \
#      -X POST \
#      -H "Transfer-Encoding: chunked" \
#      --data-binary @<(printf 'a\r\nx\r\n0\r\n\r\n') \
#      http://127.0.0.1:2500/ > /tmp/chunked_upload.txt
# RESPONSE=$(cat /tmp/chunked_upload.txt)
# if [ "$RESPONSE" != "200" ]; then
#     echo "Chunked POST failed: expected 200, got $RESPONSE"
#     kill $SERVER_PID
#     exit 1
# fi
# echo "Chunked POST passed (200)."

# Cleanup
kill $SERVER_PID
echo "✅ All tests passed."

# #!/bin/bash

# set -e

# # Start webserv in the background
# ./Webserv &
# SERVER_PID=$!

# # Give it time to boot
# sleep 1

# # test 1 GET
# RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" http://127.0.0.1:2000/)
# if [ "$RESPONSE" != "200" ]; then
#   echo "GET / failed with status $RESPONSE"
#   exit 1
# fi
# echo "GET / passed."