#!/bin/bash

set -e 
set -v

START_DIR=`pwd`

. travis_scripts/variables.sh

# Set compiler
export CXX=$CXX_COMPILER
export CC=$CC_COMPILER

# Google Test & Google Mock flags
export LDFLAGS="$LDFLAGS -L${GTEST_INSTALL_DIR}/lib -L${GMOCK_INSTALL_DIR}/lib"
export CPPFLAGS="$CPPFLAGS -I${GTEST_INSTALL_DIR}/include -I${GMOCK_INSTALL_DIR}/include"


# build libpatlms
cd libpatlms
./autogen.sh
./configure --prefix=${PATLMS_INSTALL_DIR}
make
make install
make check

# build server
cd ${START_DIR}/server
./autogen.sh
./configure --prefix=${SERVER_INSTALL_DIR}
make
make install
make check

sed -i 's~<!-- <apparmor mode="disabled"/> -->~<apparmor mode="disabled"/>~' /etc/patlms/dbus.config.template

# build agent
cd ${START_DIR}/agent
./autogen.sh
./configure --prefix=${AGENT_INSTALL_DIR}
make
make install
make check

# check web server
cd ${START_DIR}/web
tox

