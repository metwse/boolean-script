#!/bin/bash

make tests

runall() {
    for test in $(ls bin/*.test); do
        echo -n "[$(date +%H:%M:%S)] $ "
        if [[ -n "$1" ]]; then
            echo -n "$1 "
        fi
        echo "$test"
        $1 $test
    done
}

case $1 in
    "")
      echo built
      ;;
    "run")
      runall
      ;;
    "gcovr")
      runall
      gcovr *
      ;;
    "valgrind")
      runall valgrind
      ;;
esac
