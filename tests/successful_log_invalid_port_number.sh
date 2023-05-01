#!/bin/bash

# Start the webserver; uses bad config with invalid port number
./build/bin/server ./tests/test_configs/bad_port_config &

sleep 1

# check that log records invalid port number
grep "(Fatal)Invalid port number in config file" ./logs/log_0.log

# compare the output of the grep; zero indicates that the string was found in the log
if [ $? -eq 0 ]; then
    echo "success";
    kill %1
    exit 0;
else
    echo "failed";
    kill %1
    exit 1;
fi
