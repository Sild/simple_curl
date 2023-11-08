#!/bin/bash

rm -rf /tmp/habr
go run *.go https://habr.com/ /tmp/habr 4
