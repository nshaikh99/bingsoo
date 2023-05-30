#!/bin/bash

# Start the webserver; uses port 80
./build/bin/server ./conf/sample.conf &

sleep 1

curl http://localhost:80/sleep >> ./tests/result & #runs a sleep request in the background
curl http://localhost:80/echo >> ./tests/result #runs an echo request in the foreground

sleep 2 #wait for sleep request to finish

# Finds line # of the word "Slept," which is the first line of the sleep request output
SLEEP=$( grep -n "Slept" ./tests/result | grep -oE "^[0-9]+" )

# Finds line # of the word "echo," which is the first line of the echo request output
ECHO=$( grep -n "echo" ./tests/result | grep -oE "^[0-9]+" ) #

rm ./tests/result

# check that "Slept" was found after "echo" in ./tests/result, so we know echo request finished executing
# while sleep request was still executing even though sleep request was ran first
if [ $SLEEP -gt $ECHO ] 
then
  echo "success";
  kill %1
  exit 0;
else 
  echo "failure";
  kill %1
  exit 1;
fi