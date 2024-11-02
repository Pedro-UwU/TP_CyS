#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
# 
# Copyright (c) 2024 Martín E. Zahnd
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to 
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is 
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in 
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
readonly SCRIPT_DIR="$(dirname ${BASH_SOURCE[0]})"
pushd "$SCRIPT_DIR" &> /dev/null

BIN_EXEC="../stegobmp"
RM_COMMAND="trash-put"

print_success() {
  echo -en "\e[1m\e[32m[OK]\e[0m "
  echo "$1"
}

print_fail() {
  echo -en "\e[1m\e[31m[FAIL]\e[0m "
  echo "$1"
}

run_test() {
  local bmp_output="${1}"
  local msg_output="${2}"
  local original_msg_256sum=""
  local msg_output_256sum=""
  local msg_output_with_ext=""

  if [ -e "${bmp_output}" ]; then
    "${RM_COMMAND}" "${bmp_output}"
  fi

  if [ -e "${msg_output}" ]; then
    "${RM_COMMAND}" "${msg_output}"
  fi
  
  "$BIN_EXEC" -embed -in "mensaje1.txt" -p "imagen1.bmp" -out "${bmp_output}" ${3}
  if [ -e "${bmp_output}" ]; then
    print_success "Generated ${bmp_output}"
  else
    print_fail "Could not generate ${bmp_output}"
    return 1
  fi

  "$BIN_EXEC" -extract -p "${bmp_output}" -out "${msg_output}" ${3}
  msg_output_with_ext=$(compgen -G "mensaje1\.*")
  if [ -n "${msg_output_with_ext}" ]; then
    print_success "Generated ${msg_output}"
  else
    print_fail "Could not generate ${msg_output}"
    return 1
  fi

  original_msg_256sum=$(sha256sum "mensaje1.txt")
  msg_output_256sum=$(sha256sum "${msg_output_with_ext}")

  if [ "${original_msg_256sum}" = "${msg_output_256sum}" ]; then 
    print_success "SHA256 matches"
  else
    print_fail "SHA256 does NOT match"
  fi

  return 0
}

# Esteganografiar archivo con LSB1, sin encripción
run_test "lsb1_no_encript_imagenmas1.bmp" "lsb1_no_encript_mensaje1" "-steg LSB1"

 # Esteganografiar el archivo de texto “mensaje1.txt” en el archivo portador “imagen1.bmp” 
 # obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB Improved, con encripción 3DES en 
 # modo CBC con password “oculto” 
run_test "lsbi_3des_cbc_pass_imagenmas1.bmp" "lsbi_3des_cbc_pass_mensaje1" "-steg LSBI -a 3des -m cbc -pass 'oculto'"

 # Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB Improved, sin encripción
run_test "lsbi_no_encript_imagenmas1.bmp" "lsbi_no_encript_mensaje1" "-steg LSBI"


popd &> /dev/null # $SCRIPT_DIR
