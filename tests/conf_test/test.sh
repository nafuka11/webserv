#!/bin/bash

cd "$(dirname $0)"

SCRIPT_DIR=$(pwd)
EXE_PATH="../../webserv"
CONF_DIR="./conf"
EXPECTED_DIR="./expected"
ACTUAL_DIR="./actual"

COLOR_TEST="\033[36m"
COLOR_FILE_COUNT="\033[33m"
COLOR_RESET="\033[0m"

run_test () {
  file="$1"
  echo
  printf "${COLOR_TEST}test: ${file}${COLOR_RESET}\n"
  "${EXE_PATH}" "${file}" 2>&1 | tail -1
}

run_tests () {
  for file in "${array[@]}"; do
    run_test "${file}"
  done
}

show_usage () {
  echo "Usage: $0 [ Search Keyword ]"
}

main () {
if [ $# -eq 0 ] ; then
  array=($(find "${CONF_DIR}" -type f| sort))
  run_tests array
else
  array=( $(find "${CONF_DIR}" -name "*$1*.conf" | sort) )
  if [ ${#array[*]} -ne 0 ] ; then
    echo
    if [ ${#array[*]} -eq 1 ] ; then
      printf "${COLOR_FILE_COUNT}KEYWORD: \"$1\"\n▶︎ ${#array[*]} file find !${COLOR_RESET}\n"
    else
      printf "${COLOR_FILE_COUNT}KEYWORD: \"$1\"\n▶︎ ${#array[*]} files find !${COLOR_RESET}\n"
    fi
    run_tests array
  else
    show_usage
  fi
fi
}

main $@