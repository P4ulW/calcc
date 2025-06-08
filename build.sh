#! /usr/bin/zsh

DIR="$(pwd)/build"

if [ -d "$DIR" ]; then
  echo "Directory '$DIR' already exists."
else
  echo "Directory '$DIR' does not exist. Creating it now..."
  mkdir "$DIR"
fi

gcc -O2 src/main.c --std=c89 -lm -o build/calc

