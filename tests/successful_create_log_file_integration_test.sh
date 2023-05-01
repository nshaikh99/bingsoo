#!/bin/bash

# Start the webserver; uses port 80
./build/bin/server ./conf/sample.conf &

sleep 1

# check that a log file has been created
if test -f "logs/log_0.log"; then
    echo "success";
    kill %1
    exit 0;
else
    echo "failed";
    kill %1
    exit 1;
fi
