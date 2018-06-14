#!/bin/sh


sudo apt update

sudo apt --force-yes -y install git build-essential cmake libboost-all-dev libcurl4-openssl-dev libpcap-dev libssl-dev

cd deps/pcapplusplus/

./configure-linux.sh --default

cd ../..

mkdir build

cd build
