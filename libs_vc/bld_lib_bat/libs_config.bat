rem @echo off
rem This batch-file license: boost software license version 1.0
rem ==========================================================================
rem user setting

rem vc80,vc90,vc100,vc110,vc120,vc140  // 2005,2008,2010,2012,2013,2015
set CcName=vc90

rem (VC++ Express Edition: CcHasX64=0 CcNoRtStatic=1)
set CcHasX64=1
set CcNoRtStatic=

rem for glfw, libjpeg-turbo etc
set CcCMakeDir=%ProgramFiles(x86)%\CMake\bin

rem for libjpeg-turbo
set CcNasmDir=%USERPROFILE%\AppData\Local\nasm


rem ==========================================================================
rem

if "%CcName%"=="" (
  echo Please set CcName in "libs_config.bat"
  pause
  exit
)

set CcLibDir=.\lib
rem set CcLibPrefix=
set CcLibPrefix=%CcName%_
set CcLibArchX86=Win32
set CcLibStrDebug=_debug
set CcLibStrRelease=_release
set CcLibStrDll=_dll
set CcLibStrStatic=_static
set CcLibStrRtDll=

set CcMiscIncDir=misc_inc
set CcMiscLibDir=misc_lib
