@echo off
set /p instance="Enter SQL Server instance (e.g. localhost\sqlexpress): "
%WINDIR%\System32\odbcconf.exe CONFIGDSN "ODBC Driver 13 for SQL Server" "DSN=SHO|SERVER=%instance%|Trusted_Connection=Yes|Database=SHO"
%WINDIR%\System32\odbcconf.exe CONFIGDSN "ODBC Driver 13 for SQL Server" "DSN=SHO_LOG|SERVER=%instance%|Trusted_Connection=Yes|Database=SHO_LOG"
%WINDIR%\System32\odbcconf.exe CONFIGDSN "ODBC Driver 13 for SQL Server" "DSN=SHO_MALL|SERVER=%instance%|Trusted_Connection=Yes|Database=SHO_MALL"
%WINDIR%\System32\odbcconf.exe CONFIGDSN "ODBC Driver 13 for SQL Server" "DSN=seven_ORA|SERVER=%instance%|Trusted_Connection=Yes|Database=seven_ORA"
echo 4 ODBC connection created.