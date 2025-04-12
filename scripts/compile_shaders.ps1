param(
    [string]$InputDir = "assets/vulkan/shaders",
    [string]$OutputDir = "assets/vulkan/shaders/bin"
)

# Create output directory if it doesn't exist
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}

# Define shader file extensions to compile
$extensions = @("vert", "frag", "comp", "geom", "tesc", "tese")

# Set counters for statistics
$compiledCount = 0
$skippedCount = 0

foreach ($ext in $extensions) {
    Get-ChildItem -Path $InputDir -Filter "*.$ext" | ForEach-Object {
        $inputFile = $_.FullName
        $outputFile = Join-Path $OutputDir ($_.BaseName + ".$ext.spv")
        
        # Check if the output file exists and is newer than the input file
        if ((Test-Path $outputFile) -and 
            ((Get-Item $outputFile).LastWriteTime -gt (Get-Item $inputFile).LastWriteTime)) {
            Write-Host "Skipping $inputFile (already up to date)"
            $skippedCount++
        } else {
            Write-Host "Compiling $inputFile -> $outputFile"
            $result = glslangValidator -V $inputFile -o $outputFile
            if ($LASTEXITCODE -eq 0) {
                $compiledCount++
            } else {
                Write-Host "Error compiling $inputFile" -ForegroundColor Red
            }
        }
    }
}

Write-Host "Compilation complete: $compiledCount file(s) compiled, $skippedCount file(s) skipped."