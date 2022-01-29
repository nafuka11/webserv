#!/bin/bash

cd "$(dirname $0)"

SCRIPT_DIR=$(pwd)
EXE_PATH="../../webserv"
CONF_DIR="./conf"
EXPECTED_DIR="./expected"
ACTUAL_DIR="./actual"

COLOR_TEST="\033[36m"
COLOR_FILE_COUNT="\033[33m"
COLOR_OK="\033[32m"
COLOR_KO="\033[31m"
COLOR_RESET="\033[0m"

result_ok=0
result_ko=0

show_result () {
  local -i result_total=$(( result_ok + result_ko ))
  echo
  if [ "${result_ko}" -eq 0 ]; then
    printf "${COLOR_OK}"
  else
    printf "${COLOR_KO}"
  fi
  printf "${result_ok}/${result_total}${COLOR_RESET}\n"
}

show_usage () {
  echo "Usage: $0 [ Search Keyword ]"
}

assert_output () {
  local actual_file="$1"
  local expected_file="$2"
  local conf_file="$3"
  local diff_result=$(diff "${actual_file}" "${expected_file}")
  if [ $? -eq 0 ]; then
    printf "[${COLOR_OK}OK${COLOR_RESET}] ${conf_file}\n"
    (( result_ok += 1 ))
  else
    printf "[${COLOR_KO}KO${COLOR_RESET}] ${conf_file}\n"
    echo "${diff_result}"
    (( result_ko += 1 ))
  fi
}

run_test () {
  local conf_file="$1"
  local txt_file=$(basename "${conf_file}" | sed 's/\(.*\)\.[^\.]*$/\1.txt/')
  local expected_file="${EXPECTED_DIR}/${txt_file}"
  local actual_file="${ACTUAL_DIR}/${txt_file}"
  "${EXE_PATH}" "${conf_file}" 2>&1 | tail -1 > "${expected_file}"
  assert_output "${expected_file}" "${actual_file}" "${conf_file}"
}

run_tests () {
  for file in "${array[@]}"; do
    run_test "${file}"
  done
  show_result
}

main () {
  if [ $# -eq 0 ] ; then
    array=($(find "${CONF_DIR}" -type f| sort))
    run_tests array
  else
    array=( $(find "${CONF_DIR}" -name "*$1*.conf" | sort) )
    if [ ${#array[*]} -ne 0 ] ; then
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
