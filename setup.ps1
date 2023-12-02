function Usage {
    Write-Host "Usage: $PSCommandPath [daynumber]"
}

if ( $args.Count -ne 1 ) {
    Write-Host "Incorrect number of arguments"
    Usage
    Exit 1
} 

if ( (-Not ($args[0] -match "^[0-9]+$")) -or ( $args[0] -le 0 ) ) {
    Write-Host "Argument not a positive integer"
    Usage
    Exit 1
}

$daynumber=$args[0]
$filename=".\src\day{0:D2}.cpp" -f $daynumber

if ( Test-Path -Path $filename -PathType Leaf ) {
    Write-Host "File `"$filename`" already exists"
    Exit 1
}

Copy-Item -Path .\template.cpp -Destination $filename
# $replace_str=" day{0:D2}" -f $daynumber
# $content = Get-Content -Path .\Makefile
# $content[0] = $content[0] -replace "$", $replace_str
# $content | Set-Content -Path .\Makefile
