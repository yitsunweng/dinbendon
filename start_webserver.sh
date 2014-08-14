#!/bin/sh
echo "= = = Start lighttpd = = ="
sudo killall lighttpd
sudo lighttpd -f ./lighttpd.conf
