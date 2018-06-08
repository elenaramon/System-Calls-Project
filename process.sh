#! /bin/bash

ipcrm -M 75 && ipcrm -M 76 && ipcrm -Q 77 && ipcrm -S 78
rm file_output.txt
rm ./bin/key_finder
make clean
make
make install
mkdir -p mie
clear
./bin/key_finder file_input.txt file_output.txt