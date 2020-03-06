@echo off
set /p instance="Enter SQL Server instance (e.g. localhost\sqlexpress): "
%WINDIR%\System32\odbcconf.exe CONFIGDSN "SQL Server" "DSN=SHO|SERVER=%instance%|Trusted_Connection=Yes|Database=SHO"
%WINDIR%\System32\odbcconf.exe CONFIGDSN "SQL Server" "DSN=SHO_LOG|SERVER=%instance%|Trusted_Connection=Yes|Database=SHO_LOG"
%WINDIR%\System32\odbcconf.exe CONFIGDSN "SQL Server" "DSN=seven_ORA|SERVER=%instance%|Trusted_Connection=Yes|Database=seven_ORA"
echo 3 ODBC connection created.