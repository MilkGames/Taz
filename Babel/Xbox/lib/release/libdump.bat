@echo off

if "%~1"=="" (
    echo Usage: libdump.bat library.lib
    goto :eof
)

set LIBNAME=%~1

lib /list "%LIBNAME%" > obj_list.txt

for /f "delims=" %%0 in (obj_list.txt) do (
  lib /extract:%%0 %LIBNAME%
)

del obj_list.txt