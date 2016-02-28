@echo off
rem This batch-file license: boost software license version 1.0
setlocal
call libs_config.bat
cd ..

if not exist %CcMiscIncDir% mkdir %CcMiscIncDir%
if not exist %CcMiscLibDir% mkdir %CcMiscLibDir%

if not "%1"=="" set "CcLibHaruDir=%1"

if "%CcLibHaruDir%"=="" (
  for /f %%i in ('dir /b /on /ad libharu*') do set CcLibHaruDir=%%i
)

if "%CcLibHaruDir%"=="" (
  echo ERROR: not found source directory
  goto END
)

call :gen_header hpdf.h        ../%CcLibHaruDir%/include       libhpdf.lib %CcMiscIncDir%
call :gen_header hpdf_config.h ../%CcLibHaruDir%/win32/include libhpdf.lib %CcMiscIncDir%

set Arg=libcopy:%CD%\%CcMiscLibDir%
if "%CcNoRtStatic%"=="1" set Arg=%Arg% rtdll

cd %CcLibHaruDir%
call ..\bld_lib_bat\setcc.bat %CcName% %CcLibArchX86%
call ..\bld_lib_bat\bld1_libharu.bat   %CcLibArchX86% %Arg% ZlibDir:misc PngDir:misc
if "%CcHasX64%"=="1" (
  call ..\bld_lib_bat\setcc.bat %CcName% x64
  call ..\bld_lib_bat\bld1_libharu.bat x64 %Arg% ZlibDir:misc PngDir:misc
)
cd ..
goto :END

:gen_header
if not exist %4 mkdir %4
call :gen_header_print %1 %2 %3 >%4\%1
exit /b
:gen_header_print
echo /// %1 wrapper
echo #pragma once
echo #include "%2/%1"
echo #ifdef _MSC_VER
echo  #pragma comment(lib, "%3")
echo #endif
exit /b

:END
cd bld_lib_bat
endlocal
