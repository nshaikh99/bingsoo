#!/bin/bash

# Start the webserver; uses port 80
./build/bin/server ./conf/sample.conf &

sleep 1

# CRUD TEST 3: Send a DELETE request to delete previously made file in Shoes
curl -i -v -X DELETE -o ./tests/result http://localhost:80/api/Shoes/1 -s

#TODO: What is the response returned
diff --strip-trailing-cr ./tests/result ./tests/successful_crud_delete_response_integration_test_result

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