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

# Generate random file with N size
# < /dev/urandom tr -dc '[:alnum:]' |  head -c 150000000 > mensaje1_big.txt

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

# =============== LSB1 ===============

# Esteganografiar archivo con LSB1, sin encripción
run_test "lsb1_no_encript_imagenmas1.bmp" "lsb1_no_encript_mensaje1" "-steg LSB1"

# ***** AES-128 *****

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-128 y ecb; y
# password 'oculto123abc'
run_test "lsb1_aes-128-ecb_encript_imagenmas1.bmp" "lsb1_aes-128-ecb_encript_mensaje1" "-steg LSB1 -a aes128 -m ecb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-128 y CFB; y
# password 'oculto123abc'
run_test "lsb1_aes-128-cfb_encript_imagenmas1.bmp" "lsb1_aes-128-cfb_encript_mensaje1" "-steg LSB1 -a aes128 -m cfb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-128 y OFB; y
# password 'oculto123abc'
run_test "lsb1_aes-128-ofb_encript_imagenmas1.bmp" "lsb1_aes-128-ofb_encript_mensaje1" "-steg LSB1 -a aes128 -m ofb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-128 y CBC; y
# password 'oculto123abc'
run_test "lsb1_aes-128-cbc_encript_imagenmas1.bmp" "lsb1_aes-128-cbc_encript_mensaje1" "-steg LSB1 -a aes128 -m cbc -pass 'oculto123abc'"

# ***** AES-192 *****

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-192 y ecb; y
# password 'oculto123abc'
run_test "lsb1_aes-192-ecb_encript_imagenmas1.bmp" "lsb1_aes-192-ecb_encript_mensaje1" "-steg LSB1 -a aes192 -m ecb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-192 y CFB; y
# password 'oculto123abc'
run_test "lsb1_aes-192-cfb_encript_imagenmas1.bmp" "lsb1_aes-192-cfb_encript_mensaje1" "-steg LSB1 -a aes192 -m cfb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-192 y OFB; y
# password 'oculto123abc'
run_test "lsb1_aes-192-ofb_encript_imagenmas1.bmp" "lsb1_aes-192-ofb_encript_mensaje1" "-steg LSB1 -a aes192 -m ofb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-192 y CBC; y
# password 'oculto123abc'
run_test "lsb1_aes-192-cbc_encript_imagenmas1.bmp" "lsb1_aes-192-cbc_encript_mensaje1" "-steg LSB1 -a aes192 -m cbc -pass 'oculto123abc'"

# ***** AES-256 *****

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-256 y ecb; y
# password 'oculto123abc'
run_test "lsb1_aes-256-ecb_encript_imagenmas1.bmp" "lsb1_aes-256-ecb_encript_mensaje1" "-steg LSB1 -a aes256 -m ecb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-256 y CFB; y
# password 'oculto123abc'
run_test "lsb1_aes-256-cfb_encript_imagenmas1.bmp" "lsb1_aes-256-cfb_encript_mensaje1" "-steg LSB1 -a aes256 -m cfb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-256 y OFB; y
# password 'oculto123abc'
run_test "lsb1_aes-256-ofb_encript_imagenmas1.bmp" "lsb1_aes-256-ofb_encript_mensaje1" "-steg LSB1 -a aes256 -m ofb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con AES-256 y CBC; y
# password 'oculto123abc'
run_test "lsb1_aes-256-cbc_encript_imagenmas1.bmp" "lsb1_aes-256-cbc_encript_mensaje1" "-steg LSB1 -a aes256 -m cbc -pass 'oculto123abc'"

# ***** 3DES *****

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con 3DES y ecb; y
# password 'oculto123abc'
run_test "lsb1_3des-ecb_encript_imagenmas1.bmp" "lsb1_3des-ecb_encript_mensaje1" "-steg LSB1 -a 3des -m ecb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con 3DES y CFB; y
# password 'oculto123abc'
run_test "lsb1_3des-cfb_encript_imagenmas1.bmp" "lsb1_3des-cfb_encript_mensaje1" "-steg LSB1 -a 3des -m cfb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con 3DES y OFB; y
# password 'oculto123abc'
run_test "lsb1_3des-ofb_encript_imagenmas1.bmp" "lsb1_3des-ofb_encript_mensaje1" "-steg LSB1 -a 3des -m ofb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB1, con 3DES y CBC; y
# password 'oculto123abc'
run_test "lsb1_3des-cbc_encript_imagenmas1.bmp" "lsb1_3des-cbc_encript_mensaje1" "-steg LSB1 -a 3des -m cbc -pass 'oculto123abc'"


# =============== LSB4 ===============

# Esteganografiar archivo con LSB4, sin encripción
run_test "lsb4_no_encript_imagenmas1.bmp" "lsb4_no_encript_mensaje1" "-steg LSB4"

# ***** AES-128 *****

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-128 y ecb; y
# password 'oculto123abc'
run_test "lsb4_aes-128-ecb_encript_imagenmas1.bmp" "lsb4_aes-128-ecb_encript_mensaje1" "-steg LSB4 -a aes128 -m ecb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-128 y CFB; y
# password 'oculto123abc'
run_test "lsb4_aes-128-cfb_encript_imagenmas1.bmp" "lsb4_aes-128-cfb_encript_mensaje1" "-steg LSB4 -a aes128 -m cfb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-128 y OFB; y
# password 'oculto123abc'
run_test "lsb4_aes-128-ofb_encript_imagenmas1.bmp" "lsb4_aes-128-ofb_encript_mensaje1" "-steg LSB4 -a aes128 -m ofb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-128 y CBC; y
# password 'oculto123abc'
run_test "lsb4_aes-128-cbc_encript_imagenmas1.bmp" "lsb4_aes-128-cbc_encript_mensaje1" "-steg LSB4 -a aes128 -m cbc -pass 'oculto123abc'"

# ***** AES-192 *****

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-192 y ecb; y
# password 'oculto123abc'
run_test "lsb4_aes-192-ecb_encript_imagenmas1.bmp" "lsb4_aes-192-ecb_encript_mensaje1" "-steg LSB4 -a aes192 -m ecb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-192 y CFB; y
# password 'oculto123abc'
run_test "lsb4_aes-192-cfb_encript_imagenmas1.bmp" "lsb4_aes-192-cfb_encript_mensaje1" "-steg LSB4 -a aes192 -m cfb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-192 y OFB; y
# password 'oculto123abc'
run_test "lsb4_aes-192-ofb_encript_imagenmas1.bmp" "lsb4_aes-192-ofb_encript_mensaje1" "-steg LSB4 -a aes192 -m ofb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-192 y CBC; y
# password 'oculto123abc'
run_test "lsb4_aes-192-cbc_encript_imagenmas1.bmp" "lsb4_aes-192-cbc_encript_mensaje1" "-steg LSB4 -a aes192 -m cbc -pass 'oculto123abc'"

# ***** AES-256 *****

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-256 y ecb; y
# password 'oculto123abc'
run_test "lsb4_aes-256-ecb_encript_imagenmas1.bmp" "lsb4_aes-256-ecb_encript_mensaje1" "-steg LSB4 -a aes256 -m ecb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-256 y CFB; y
# password 'oculto123abc'
run_test "lsb4_aes-256-cfb_encript_imagenmas1.bmp" "lsb4_aes-256-cfb_encript_mensaje1" "-steg LSB4 -a aes256 -m cfb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-256 y OFB; y
# password 'oculto123abc'
run_test "lsb4_aes-256-ofb_encript_imagenmas1.bmp" "lsb4_aes-256-ofb_encript_mensaje1" "-steg LSB4 -a aes256 -m ofb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con AES-256 y CBC; y
# password 'oculto123abc'
run_test "lsb4_aes-256-cbc_encript_imagenmas1.bmp" "lsb4_aes-256-cbc_encript_mensaje1" "-steg LSB4 -a aes256 -m cbc -pass 'oculto123abc'"

# ***** 3DES *****

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con 3DES y ecb; y
# password 'oculto123abc'
run_test "lsb4_3des-ecb_encript_imagenmas1.bmp" "lsb4_3des-ecb_encript_mensaje1" "-steg LSB4 -a 3des -m ecb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con 3DES y CFB; y
# password 'oculto123abc'
run_test "lsb4_3des-cfb_encript_imagenmas1.bmp" "lsb4_3des-cfb_encript_mensaje1" "-steg LSB4 -a 3des -m cfb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con 3DES y OFB; y
# password 'oculto123abc'
run_test "lsb4_3des-ofb_encript_imagenmas1.bmp" "lsb4_3des-ofb_encript_mensaje1" "-steg LSB4 -a 3des -m ofb -pass 'oculto123abc'"

# Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB4, con 3DES y CBC; y
# password 'oculto123abc'
run_test "lsb4_3des-cbc_encript_imagenmas1.bmp" "lsb4_3des-cbc_encript_mensaje1" "-steg LSB4 -a 3des -m cbc -pass 'oculto123abc'"

# =============== LSBI ===============

# # Esteganografiar el archivo de texto “mensaje1.txt” en el archivo portador “imagen1.bmp” 
# # obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB Improved, con encripción 3DES en 
# # modo CBC con password “oculto” 
# run_test "lsbi_3des_cbc_pass_imagenmas1.bmp" "lsbi_3des_cbc_pass_mensaje1" "-steg LSBI -a 3des -m cbc -pass 'oculto'"
# 
# # Esteganografiar el archivo de imagen “mensaje1.txt” en el archivo portador “imagen1 .bmp” 
# # obteniendo un archivo “imagenmas1.bmp” mediante el algoritmo LSB Improved, sin encripción
# run_test "lsbi_no_encript_imagenmas1.bmp" "lsbi_no_encript_mensaje1" "-steg LSBI"


popd &> /dev/null # $SCRIPT_DIR
