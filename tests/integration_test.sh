#!/bin/bash

# Start the webserver; uses port 80
./bin/server tests/test_configs/example_config &
PID=$!

sleep 1

curl http://localhost:80 -s -o ./tests/result

diff ./tests/result ./tests/expected_integration_test_result

if [[ $? -eq 0 ]]; then
    echo "success"; 
    kill %1
    rm ./tests/result
    exit 0;
else 
    echo "failed"; 
    kill %1
    rm ./tests/result
    exit 1;
fi