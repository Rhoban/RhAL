#!/bin/bash

cd Vendors

if [ ! -d "serial" ]; then
    git clone https://github.com/Rhoban/serial.git
else
    cd serial
    git pull
    cd ..
fi

if [ ! -d "json" ]; then
    git clone https://github.com/open-source-parsers/json.git
else
    cd json
    git pull
    cd ..
fi

