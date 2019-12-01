@echo off
set SERVER_DATA=%~dp0\..\server\data\3ddata

IF NOT EXIST %SERVER_DATA% (
	mkdir %SERVER_DATA%\..
	mklink /J %SERVER_DATA% %~dp0..\game\3ddata
) ELSE (
	echo %SERVER_DATA% already exists
)


pause