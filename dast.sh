#!/bin/bash
set -e

./Webserv &

WEBSERV_PID=$!

# echo "Waiting for Webserv to start on port 8080..."
# for i in {1..30}; do
#     if nc -z localhost 8080; then
#         echo "Webserv is up!"
#         break
#     fi
#     sleep 1
# done

# if ! nc -z localhost 8080; then
#     echo "Error: Webserv did not start within 30 seconds."
#     kill $WEBSERV_PID
#     exit 1
# fi

# zap-baseline.py -t http://localhost:8080 -r zap_report.html

# echo "ZAP scan report saved to zap_report.html"

# kill $WEBSERV_PID

# exit 0
./Webserv
