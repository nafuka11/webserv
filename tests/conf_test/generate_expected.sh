#!/bin/bash

cd "$(dirname $0)"

readonly SCRIPT_DIR=$(pwd)
readonly EXE_PATH="../../webserv"
readonly CONF_DIR="./conf"
readonly EXPECTED_DIR="./expected"

readonly COLOR_FILE_COUNT="\033[33m"
readonly COLOR_RESET="\033[0m"

show_search_result () {
  local keyword="$1"
  local -i count="$2"
  printf "${COLOR_FILE_COUNT}KEYWORD: \"${keyword}\"\n▶︎ ${count} file"
  if [ ${count} -ne 1 ]; then
    echo -n "s"
  fi
  printf " found !${COLOR_RESET}\n"
  echo
}

show_usage () {
  echo "Usage: $0 [ Search Keyword ]"
}

cleanup_expected_files () {
  rm -f "${EXPECTED_DIR}/*.txt"
}

dump_expected_file () {
  local conf_file="$1"
  local txt_file=$(basename "${conf_file}" | sed 's/\(.*\)\.[^\.]*$/\1.txt/')
  local expected_file="${EXPECTED_DIR}/${txt_file}"
  "${EXE_PATH}" "${conf_file}" 2>&1 | tail -1 > "${expected_file}"
  printf "dumped: ${expected_file}\n"
}

dump_expected_files () {
  cleanup_expected_files
  for file in "${array[@]}"; do
    dump_expected_file "${file}"
  done
}

main () {
  if [ $# -eq 0 ] ; then
    array=($(find "${CONF_DIR}" -type f| sort))
    dump_expected_files array
  else
    array=($(find "${CONF_DIR}" -name "*$1*.conf" | sort))
    if [ ${#array[*]} -ne 0 ] ; then
      show_search_result "$1" ${#array[*]}
      dump_expected_files array
    else
      show_usage
    fi
  fi
}

main $@
