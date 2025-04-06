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

foreach ($ext in $extensions) {
    Get-ChildItem -Path $InputDir -Filter "*.$ext" | ForEach-Object {
        $inputFile = $_.FullName
        $outputFile = Join-Path $OutputDir ($_.BaseName + ".$ext.spv")
        Write-Host "Compiling $inputFile -> $outputFile"
        glslangValidator -V $inputFile -o $outputFile
    }
}
