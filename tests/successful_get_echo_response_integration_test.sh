#!/bin/bash

# start the webserver; uses port 80
./build/bin/server ./conf/sample.conf &
sleep 1

# send a request to the server
curl -i -v -o ./tests/result http://localhost:80/echo -s

# compare the output from the server with the expected output
diff ./tests/result ./tests/successful_get_echo_response_integration_test_result

# compare the output of the diff; zero indicates no difference in the diff
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
