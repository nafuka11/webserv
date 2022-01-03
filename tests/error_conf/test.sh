#!/bin/bash

EXE_PATH="./webserv"
CONF_DIR="./conf/error"
COLOR_TEST="\033[36m"
COLOR_RESET="\033[0m"

if [ $# -eq 0 -o "$1" = "all" ] ; then ## Test all config error file
  for file in $(find "${CONF_DIR}" -name "*.conf" | sort); do
    echo
    printf "${COLOR_TEST}test: ${file}${COLOR_RESET}\n"
    "${EXE_PATH}" "${file}" 2>&1 | tail -1
  done
elif [ "$1" = "semicolon" ] ; then ## Test error for semicolon(;)
  for file in $(find "${CONF_DIR}" -name "*_semicolon.conf" | sort); do
    echo
    printf "${COLOR_TEST}test: ${file}${COLOR_RESET}\n"
    "${EXE_PATH}" "${file}" 2>&1 | tail -1
  done
elif [ "$1" = "help" ] ; then ## Show this message
  grep 'if \[' tests/error_conf/test.sh | awk -F'\"|## ' '{printf"    \033[36m%-10s\033[0m %s\n", $4, $6}'
else
  :
fi
