#!/usr/bin/env bash

INPUT_IMAGE="${1}"

if [ -z "${INPUT_IMAGE}" ]; then
  echo -e "\e[1mUsage:\e[0m"
  echo -e "${0} \e[4mimage_to_convert\e[0m"
  exit 1
fi

# Choose from: Bilevel, Grayscale, GrayscaleAlpha, Palette,
# PaletteAlpha, TrueColor, TrueColorAlpha, ColorSeparation, 
# or ColorSeparationAlpha.
#
# TrueColor seems to force 24-bit

magick "${INPUT_IMAGE}" \
  -define bmp:format=bmp3 \
  -compress none \
  -type TrueColor \
  "${INPUT_IMAGE%%.*}.bmp"
