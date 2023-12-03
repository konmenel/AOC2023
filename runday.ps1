function Usage() {
    Write-Host "Usage:"
    Write-Host "$PSCommandPath <daynumber> [release(default)|debug]"
}

if ( $args.Count -lt 1 ) {
    Write-Host "Incorrect number of arguments"
    Usage
    Exit 1
} 

$daynumber=$args[0]
$mode=$args[1]

if ( (-Not ($daynumber -match "^[0-9]+$")) -or ( $args[0] -le 0 ) ) {
    Write-Host "Argument not a positive integer"
    Usage
    Exit 1
}

$daynumber="{0:D2}" -f $daynumber

$debug=$false
$gcc_flacs=""
$datadir="data"
if ( "debug" -eq $mode ) {
    $debug=$true
    $gcc_flacs="-DDEBUG -O0 -g"
    $datadir="data/examples"
} elseif (("release" -eq $mode) -or (-Not $mode)) {
    $debug=$false
    $gcc_flacs="-O3"
    $datadir="data"
} else {
    Write-Host "Unknown mode \"$mode\""
    Usage
    Exit 1
}

Invoke-Expression "g++ ./src/day$daynumber.cpp -o ./bin/day$daynumber $gcc_flacs"
Invoke-Expression "./bin/day$daynumber $datadir/$daynumber.txt"
