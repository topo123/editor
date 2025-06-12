#!/bin/bash
set -e

if [ $# -eq 0 ]; then
	for file in ./build-tests/tests/*; do
		if [ -f $file ] && [ -x $file ]; then 
			./$file
		fi
	done
elif [ -x "./build-tests/tests/${1}_test" ]; then
	./build-tests/tests/${1}_test
fi


