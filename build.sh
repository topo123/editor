#!/bin/bash
set -e

if [[ ! -d "build-tests" && "$1" = "tests" ]]; then
	cmake -B build-tests -G Ninja -DBUILD_MAIN=OFF -DBUILD_TESTS=ON -DBUILD_EVERYTHING=OFF
elif [[ ! -d "build" && "$1" = "main" ]]; then
	cmake -B build -G Ninja -DBUILD_MAIN=ON -DBUILD_TESTS=OFF -DBUILD_EVERYTHING=OFF
elif [[ (! -d "build" || ! -d "build-tests") && "$1" = "all" ]]; then
	cmake -B build -G Ninja -DBUILD_MAIN=OFF -DBUILD_TESTS=OFF -DBUILD_EVERYTHING=ON
fi

if [ "$1" = "tests" ]; then
	ninja -C build-tests 
elif [ "$1" = "main" ]; then
	ninja -C build
elif [ "$1" = "all" ]; then
	ninja -C build
	ninja -C build-tests
fi
