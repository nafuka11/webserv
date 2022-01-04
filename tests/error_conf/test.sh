#!/bin/bash

EXE_PATH="./webserv"
CONF_DIR="./tests/error_conf/conf/"
COLOR_TEST="\033[36m"
COLOR_RESET="\033[0m"

run_test () {
  file="$1"
  echo
  printf "${COLOR_TEST}test: ${file}${COLOR_RESET}\n"
  "${EXE_PATH}" "${file}" 2>&1 | tail -1
}

run_tests () {
  for file in $(find "${CONF_DIR}" -name "$1" | sort); do
    run_test "${file}"
  done
}

show_usage () {
  grep 'if \[' tests/error_conf/test.sh | awk -F'\"|## ' '{printf"    \033[36m%-10s\033[0m %s\n", $4, $6}'
}

main () {
if [ $# -eq 0 -o "$1" = "all" ] ; then ## Test all config error file
  run_tests "*.conf"
elif [ "$1" = "context" ] ; then ## Test error for context(server, location)
  run_tests "context_*.conf"
elif [ "$1" = "directive" ] ; then ## Test error for directive
  run_tests "directive_*.conf"
elif [ "$1" = "semicolon" ] ; then ## Test error for semicolon(;)
  run_tests "*semicolon_*.conf"
elif [ "$1" = "help" ] ; then ## Show this message
  show_usage
else
  show_usage
fi
}

main $@