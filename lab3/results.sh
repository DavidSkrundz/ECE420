#!/usr/bin/env bash

make test-clean && make && cp data_1k data_input
python2 results.py > results_1k.txt

make test-clean && make && cp data_2k data_input
python2 results.py > results_2k.txt

make test-clean && make && cp data_4k data_input
python2 results.py > results_4k.txt

touch done
