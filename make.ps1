param (
    [switch]$run = $false,
    [switch]$build = $false,
    [switch]$debug = $true
)

$CC = "clang"
$CFLAGS = "-Wall -Wextra"
$DEBUG_FLAGS = "-DDEBUG -g"
$RELEASE_FLAGS = "-DNDEBUG -O3"
$IFLAGS = "-I.\vendor\include"

if($debug) {
    $CFLAGS = "$CFLAGS $DEBUG_FLAGS"
} else {
    $CFLAGS = "$CFLAGS $RELEASE_FLAGS"
}

function build {
    New-Item -Path "build" -ItemType Directory -Force
    $compile = "$CC .\src\main.c -o .\build\main.exe $IFLAGS $CFLAGS"
    Invoke-Expression $compile
}

function run {
    & ".\build\main.exe"
}

if ($run) {
    run
} elseif ($build) {
    build
} else {
    build
    run
}


