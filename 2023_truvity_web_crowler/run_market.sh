#!/bin/bash

rm -rf /tmp/market
go run *.go https://market.yandex.ru /tmp/market 4
