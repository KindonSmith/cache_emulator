$output = "results_alloc.txt"
$trace = "input/trace_rw7.txt"
$configs = Get-ChildItem -Path "config/assoc_increase" -File

Clear-Content -Path $output -ErrorAction SilentlyContinue

foreach ($config in $configs) {
    $configPath = "config/assoc_increase/$($config.Name)"
    Add-Content -Path $output -Value "=== Config: $($config.Name) ==="
    $result = ./amat.exe --config $configPath --input $trace
    Add-Content -Path $output -Value $result
    Add-Content -Path $output -Value ""
}

Write-Host "Done. Results written to $output"
