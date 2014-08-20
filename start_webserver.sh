#!/bin/sh
make clean
make
echo "= = = Start lighttpd = = ="
sudo killall lighttpd
sudo lighttpd -f ./lighttpd.conf
