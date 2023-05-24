#!/bin/bash

# Start the webserver; uses port 80
./build/bin/server ./conf/sample.conf &

sleep 1

# CRUD TEST 2: Send a GET request to retrieve the previously made file in Shoes
curl -i -v -X GET -o ./tests/result http://localhost:80/api/Shoes/1 -s

diff --strip-trailing-cr ./tests/result ./tests/successful_crud_get_response_integration_test_result

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