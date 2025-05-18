#!/bin/sh

dir="$(dirname ${BASH_SOURCE})"

for f in $*; do
    xxd -i "${f}"
done #> $dir/blob.h

