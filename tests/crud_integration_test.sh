#!/bin/bash

# Start the webserver; uses port 80
./build/bin/server ./conf/ass7.conf &

sleep 1

########### TEST 1 #################

echo "CRUD TEST 1: Send POST to create a new entity"

json_data="{\"color\": \"black\", \"size\": 7, \"brand\": \"Adidas\"}"

curl -s -i -v -X POST -H "Content-Type: application/json" -d "$json_data" -o ./tests/result http://localhost:80/api/Shoes

# Gets id from id_line in response
id=$(cat ./tests/result | grep -o '{"id": [^}]*}' | grep -oP '(?<="id": )\d+')

# Verifies that id is a number
if [[ $id =~ ^[0-9]+$ ]]; then
    echo "success - POST returns a valid id."
    rm ./tests/result
else
    echo "failed - POST did not return a valid id."
    rm ./tests/result
    kill %1
    exit 1;
fi

########### TEST 2 #################

echo "CRUD TEST 2: Send GET to retrieve the newly-created entity"
curl -s -i -v -X GET -o ./tests/result http://localhost:80/api/Shoes/$id

# Get json body from response
response_data=$(cat ./tests/result | grep -o '{.*}')

if [ "$response_data" = "$json_data" ]; then
    echo "success - GET returned expected json";
    rm ./tests/result
else
    echo "failed - GET did not return the expected json";
    rm ./tests/result
    kill %1
    exit 1;
fi

# ########### TEST 3 #################

echo "CRUD TEST 3: Send DELETE to delete the newly-created entity"
curl -s -i -v -X DELETE -o ./tests/result http://localhost:80/api/Shoes/$id

# Get id from file path of response
response_data=$(cat ./tests/result | grep -oE '/Shoes/[0-9]+' | grep -oE '[0-9]+') 

if [ "$response_data" = "$id" ]; then
    echo "success - DELETE returned expected json for deleted entity";
    rm ./tests/result
else
    echo "failed - DELETE did not return the expected json for deleted entity";
    rm ./tests/result
    kill %1
    exit 1;
fi

########### TEST 4 #################

echo "CRUD TEST 4: Send GET to verify that entity has been deleted"
curl -s -i -X GET -o ./tests/result http://localhost:80/api/Shoes/$id

diff --strip-trailing-cr ./tests/result ./tests/crud_integration_test_404_result

# compare the output of the diff; zero indicates no difference in the diff
if [[ $? -eq 0 ]]; then
    echo "success - GET returned expected json";
    rm ./tests/result
    kill %1
    exit 0;
else
    echo "failed - GET did not return the expected json";
    rm ./tests/result
    kill %1
    exit 1;
fi