#!/bin/bash

set -e

if [ `basename $PWD` != "src" ]; then
    pushd priv/src
fi

unset CFLAGS LDFLAGS

make $1