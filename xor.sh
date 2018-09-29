#!/bin/bash
make
./xor -e pepperandsalt -v cucumbersalat withoutTomato -iinput.txt -oencrypted.txt
./xor -d pepperandsalt -v cucumbersalat withoutTomato -iencrypted.txt -odecrypted.txt
