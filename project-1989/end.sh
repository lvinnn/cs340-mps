#!/bin/bash

kill -9 $(lsof -i:5000 -t) 2> /dev/null
kill -9 $(lsof -i:5001 -t) 2> /dev/null
kill -9 $(lsof -i:5002 -t) 2> /dev/null
kill -9 $(lsof -i:5003 -t) 2> /dev/null
kill -9 $(lsof -i:5004 -t) 2> /dev/null
kill -9 $(lsof -i:5005 -t) 2> /dev/null