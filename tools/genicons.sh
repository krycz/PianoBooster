#!/bin/bash
set -e

icon=${1:-logo/Icon4.svg}

# make icons for GNU/Linux application entry
mkdir -p "icons/hicolor/scalable"
cp "$icon" "icons/hicolor/scalable/pianobooster.svg" # cannot be a symlink
for size in 32 48 64 256; do
    mkdir -p "icons/hicolor/${size}x${size}"
    ffmpeg -y -i "$icon" -vf scale="$size:$size" "icons/hicolor/${size}x${size}/pianobooster.png"
done

# make icon for MacOS bundle (requires png2icns from libicns package)
for size in 32 48 256; do
    pngs+=("icons/hicolor/${size}x${size}/pianobooster.png")
done
png2icns "icons/MacOS/pianobooster.icns" "${pngs[@]}"

# make icons for application itself
size=256
for format in ico png; do
    ffmpeg -y -i "$icon" -vf scale="$size:$size" "src/images/pianobooster.$format"
done
ln -srf "icons/MacOS/pianobooster.icns" "src/images/pianobooster.icns"
