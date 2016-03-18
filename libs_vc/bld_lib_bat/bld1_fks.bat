@echo off
rem Compile fks for vc
rem usage: bld1_fks [x86/x64] [debug/release] [static/rtdll] [libdir:DEST_DIR]
rem ex)
rem cd fks
rem ..\bld_lib_bat\bld1_fks.bat x64 static libdir:d:\mylib
rem
rem This batch-file license: boost software license version 1.0
setlocal

set ToolSet=
set Arch=
set Platform=
rem set LibDir=
rem set StrPrefix=
rem set StrRel=_release
rem set StrDbg=_debug
rem set StrRtSta=_static
rem set StrRtDll=_rtdll

rem set Arch=%CcArch%
rem set LibDir=%CcLibDir%
set LibDir=
set StrPrefix=%CcLibPrefix%
set StrRel=%CcLibStrRelease%
set StrDbg=%CcLibStrDebug%
set StrRtSta=%CcLibStrStatic%
set StrRtDll=%CcLibStrRtDll%

set LibCopyDir=
set HasRel=
set HasDbg=
set HasRtSta=
set HasRtDll=

:ARG_LOOP
  if "%1"=="" goto ARG_LOOP_EXIT

  if /I "%1"=="x86"      set Arch=x86
  if /I "%1"=="win32"    set Arch=x86
  if /I "%1"=="x64"      set Arch=x64

  if /I "%1"=="static"   set HasRtSta=S
  if /I "%1"=="rtsta"    set HasRtSta=S
  if /I "%1"=="rtdll"    set HasRtDll=L

  if /I "%1"=="release"  set HasRel=r
  if /I "%1"=="debug"    set HasDbg=d

  set ARG=%1
  if /I "%ARG:~0,8%"=="LibCopy:"    set LibCopyDir=%ARG:~8%
  if /I "%ARG:~0,7%"=="LibDir:"     set LibDir=%ARG:~7%
  if /I "%ARG:~0,10%"=="LibPrefix:" set StrPrefix=%ARG:~10%
  if /I "%ARG:~0,9%"=="LibRtSta:"   set StrRtSta=%ARG:~9%
  if /I "%ARG:~0,9%"=="LibRtDll:"   set StrRtDll=%ARG:~9%
  if /I "%ARG:~0,7%"=="LibRel:"     set StrRel=%ARG:~7%
  if /I "%ARG:~0,7%"=="LibDbg:"     set StrDbg=%ARG:~7%

  shift
goto ARG_LOOP
:ARG_LOOP_EXIT

if "%ToolSet%"=="" (
  if /I not "%PATH:Microsoft Visual Studio 14.0\VC=%"=="%PATH%" set ToolSet=vc140
  if /I not "%PATH:Microsoft Visual Studio 13.0\VC=%"=="%PATH%" set ToolSet=vc130
  if /I not "%PATH:Microsoft Visual Studio 12.0\VC=%"=="%PATH%" set ToolSet=vc120
  if /I not "%PATH:Microsoft Visual Studio 11.0\VC=%"=="%PATH%" set ToolSet=vc110
  if /I not "%PATH:Microsoft Visual Studio 10.0\VC=%"=="%PATH%" set ToolSet=vc100
  if /I not "%PATH:Microsoft Visual Studio 9.0\VC=%"=="%PATH%"  set ToolSet=vc90
  if /I not "%PATH:Microsoft Visual Studio 8\VC=%"=="%PATH%"    set ToolSet=vc80
)
if "%ToolSet%"=="" set ToolSet=vc
if "%StrPrefix%"=="" set StrPrefix=%ToolSet%_

if "%Arch%"=="" (
  if /I not "%PATH:Microsoft Visual Studio 14.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
  if /I not "%PATH:Microsoft Visual Studio 13.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
  if /I not "%PATH:Microsoft Visual Studio 12.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
  if /I not "%PATH:Microsoft Visual Studio 11.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
  if /I not "%PATH:Microsoft Visual Studio 10.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
  if /I not "%PATH:Microsoft Visual Studio 9.0\VC\BIN\amd64=%"=="%PATH%"  set Arch=x64
  if /I not "%PATH:Microsoft Visual Studio 8\VC\BIN\amd64=%"=="%PATH%"    set Arch=x64
)
if "%Arch%"=="" (
  set Arch=x86
)
if "%Arch%"=="x86" (
  set Platform=Win32
) else (
  set Platform=%Arch%
)

if "%HasRtSta%%HasRtDll%"=="" (
  set HasRtSta=S
  rem set HasRtDll=L
)
if "%HasRel%%HasDbg%"=="" (
  set HasRel=r
  rem set HasDbg=d
)

if "%StrRel%%StrDbg%"==""     set StrDbg=_debug
if "%StrRtSta%%StrRtDll%"=="" set StrRtSta=_static

if "%LibDir%"=="" set LibDir=lib_no_fkstd
rem if not exist %LibDir% mkdir %LibDir%

if "%HasRtSta%%HasRel%"=="Sr" call :Bld1 rtsta rel %StrPrefix%%Platform%%StrRtSta%%StrRel%
if "%HasRtSta%%HasDbg%"=="Sd" call :Bld1 rtsta dbg %StrPrefix%%Platform%%StrRtSta%%StrDbg%
if "%HasRtDll%%HasRel%"=="Lr" call :Bld1 rtdll rel %StrPrefix%%Platform%%StrRtDll%%StrRel%
if "%HasRtDll%%HasDbg%"=="Ld" call :Bld1 rtdll dbg %StrPrefix%%Platform%%StrRtDll%%StrDbg%

endlocal
goto :EOF


:Bld1
set RtType=%1
set BldType=%2
set Target=%3

set "cflags=-c -W3 -D_CRT_SECURE_NO_WARNINGS -wd4996 -I. -Ifks -Iimpl -Iimpl/fks_impl -Iimpl/fkstd_impl -Iimpl/fks_impl/mswin -Iimpl/fkstd_impl/mswin"
set cflags=-DFKS_NO_FKSTD %cflags%
set "conlflags=/INCREMENTAL:NO /NOLOGO -subsystem:console,5.01"
set "conlibs=kernel32.lib advapi32.lib"

if "%RtType%"=="rtdll" (
  set RtOpts=-MD
) else (
  set RtOpts=-MT
)
if "%BldType%"=="dbg" (
  set BldOpts=-O2 -Zi
  set ldebug=/DEBUG
  set RtOpts=%RtOpts%d
) else (
  set BldOpts=-Ox -DNDEBUG
  set ldebug=/RELEASE
)

rem pushd ..

set BldDir=bld
if not exist %BldDir% mkdir %BldDir%
set WkDir=%BldDir%\obj\%Target%
if not exist %BldDir%\obj mkdir %BldDir%\obj
if not exist %WkDir% mkdir %WkDir%


set TgtLibDir=%LibDir%\%Target%
if not exist %LibDir% mkdir %LibDir%
if not exist %TgtLibDir% mkdir %TgtLibDir%

rem goto SKIP_MAKE_LIB

rem [COMPILE]
del %WkDir%\*.obj
for /R impl\fks_impl   %%i in (*.c *.cpp) do call :Compile1 %%i %WkDir%\%%~ni.obj
rem for /R impl\fkstd_impl %%i in (*.c *.cpp) do call :Compile1 %%i %WkDir%\%%~ni.obj

rem [MAKE LIBRARY]
set TmpObjsFile=%WkDir%\tmp_%Target%_lnk.lst
del %TmpObjsFile%
for /R %WkDir% %%i in (*.obj) do echo %%i | sort >>%TmpObjsFile%
lib /OUT:%TgtLibDir%\fks.lib /SUBSYSTEM:windows /NOLOGO /MACHINE:%Arch% @%TmpObjsFile%
:SKIP_MAKE_LIB


goto SKIP_TEST
rem [TEST]
set TstWkDir=%BldDir%\obj\tst_%Target%
if not exist %TstWkDir% mkdir %TstWkDir%

set TstExeDir=%BldDir%\tst_exe
if not exist %TstExeDir% mkdir %TstExeDir%

set cflags=%cflags% -DFKS_USE_TEST
for /R test\test_fks %%i in (*.c *.cpp) do call :Compile1 %%i %TstWkDir%\%%~ni.obj
set TstObjsFile=%TstWkDir%\test_%Target%_lnk.lst
del %TstObjsFile%
for /R %TstWkDir% %%i in (*.obj) do echo %%i | sort >>%TstObjsFile%
cl -Fe%TstExeDir%\test_%Target%.exe @%TstObjsFile% %TgtLibDir%\fks.lib
:SKIP_TEST

if "%LibCopyDir%"=="" goto ENDIF_LibCopyDir
if not exist %LibCopyDir% mkdir %LibCopyDir%
if not exist %LibCopyDir%\%Target% mkdir %LibCopyDir%\%Target%
if exist %TgtLibDir%\*.lib copy %TgtLibDir%\*.lib %LibCopyDir%\%Target%
if exist %TgtLibDir%\*.pdb copy %TgtLibDir%\*.pdb %LibCopyDir%\%Target%
:ENDIF_LibCopyDir

rem popd

goto :EOF

:Compile1
cl %cflags% -Fo%2 %1
exit /b


