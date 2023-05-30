#!/bin/bash

# Start the webserver; uses port 80
./build/bin/server ./conf/sample.conf &

sleep 1

curl http://localhost:80/sleep >> ./tests/result &
curl http://localhost:80/echo >> ./tests/result

sleep 2

SLEEP=$( grep -n "Slept" ./tests/result | grep -oE "^[0-9]+" )
ECHO=$( grep -n "echo" ./tests/result | grep -oE "^[0-9]+" )

rm ./tests/result

#Need slept to appear after echo
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