#!/bin/bash
TEST=http://localhost:8080
curl -i -X DELETE http://localhost:8080/test.txt
curl -i -X GET $TEST
