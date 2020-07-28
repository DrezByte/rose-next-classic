param (
    [string]$pg_database = "squash_migrations",
    [string]$pg_host = "localhost",
    [int]$pg_port = 5432,
    [string]$pg_user = "postgres",
    [string]$pg_password = "postgres",
    [string]$out = (Join-Path $PSScriptRoot .. database),
    [string]$out_name = "rose-next"
)

$pg_root = "C:/Program Files/PostgreSQL/12/bin"
$pg_createdb = Join-Path $pg_root createdb.exe
$pg_dropdb = Join-Path $pg_root dropdb.exe
$pg_dump = Join-Path $pg_root pg_dump.exe
$psql = Join-Path $pg_root  psql.exe
$pg_flags = @(
    "--schema-only",
    "--clean",
    "--if-exists",
    "--no-privileges",
    "--no-owner",
    "--no-tablespaces",
    "--quote-all-identifiers"
)

$env:PGDATABASE = $pg_database
$env:PGHOST = $pg_host
$env:PGPORT = $pg_port
$env:PGUSER = $pg_user
$env:PGPASSWORD = $pg_password

$rose_next_root = Join-Path $PSScriptRoot ..
$db_root = Join-Path $rose_next_root database

# Account database
$account_db_root = Join-Path $db_root migrations account
$account_migrations = Get-ChildItem -Path $account_db_root -Directory | Sort-Object
$account_out = Join-Path $out ($out_name + "-account.sql")

Write-Host "Executing account database migrations"

& $pg_createdb $pg_database
Foreach ($migration in $account_migrations) {
    & $psql -f (Join-Path $migration up.sql)
}

Write-Host "Dumping account database to $account_out"

& $pg_dump @pg_flags -f $account_out $pg_database 
& $pg_dropdb $pg_database


# game database
$game_db_root = Join-Path $db_root migrations game
$game_migrations = Get-ChildItem -Path $game_db_root -Directory | Sort-Object
$game_out = Join-Path $out ($out_name + ".sql")

Write-Host "Executing game database migrations"

& $pg_createdb $pg_database
Foreach ($migration in $game_migrations) {
    & $psql -f (Join-Path $migration up.sql)
}

Write-Host "Dumping game database to $game_out"

& $pg_dump @pg_flags -f $game_out $pg_database 
& $pg_dropdb $pg_database

