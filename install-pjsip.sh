#!/bin/sh

# Build and install pjsip for Travis CI builds, which are based on Ubuntu 
# 12.04 machines. Normally developers should use the pre-packaged version
# of these libraries.

set -ex
wget http://www.pjsip.org/release/2.3/pjproject-2.3.tar.bz2 -O /tmp/pjproject.tar.bz2
tar xf /tmp/pjproject.tar.bz2 -C /tmp/
cd /tmp/pjproject-2.3 && ./configure --prefix=/usr && make && sudo make install
