#!/bin/bash

# Accept input and output directories as parameters
inputDir="${1:-assets/vulkan/shaders}"
outputDir="${2:-assets/vulkan/shaders/bin}"

# Create output directory if it doesn't exist
mkdir -p "$outputDir"

# Define shader file extensions to compile
extensions=("vert" "frag" "comp" "geom" "tesc" "tese")

for ext in "${extensions[@]}"; do
    for file in "$inputDir"/*."$ext"; do
        [ -e "$file" ] || continue
        filename=$(basename "$file")
        base="${filename%.*}"
        outputFile="$outputDir/${base}.${ext}.spv"
        echo "Compiling $file -> $outputFile"
        glslangValidator -V "$file" -o "$outputFile"
    done
done
