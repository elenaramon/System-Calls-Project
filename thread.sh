#! /bin/bash

ipcrm -M 75 && ipcrm -M 76 && ipcrm -Q 77 && ipcrm -S 78
rm file_output.txt
rm ./bin/key_finder
make clean
make thread
make install
clear
./bin/key_finder file_input.txt file_output.txt
