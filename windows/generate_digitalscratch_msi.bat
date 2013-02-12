echo off
SETLOCAL EnableDelayedExpansion
REM
REM Tools for packaging DigitalScratch software as a setup installation file.
REM

REM
REM Check parameters
REM
IF [%1]==[] (
  echo ERROR: Incorrect number of mandatory parameters !
  GOTO usage
)

REM
REM Get MSI file name
REM
SET OUTPUT_MSI=%1

echo.
echo ----------------------------------------------------------------------------------
echo  Setup environment ...
echo ----------------------------------------------------------------------------------
echo.
call "C:\Qt\4.8.4\bin\qtvars.bat"
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"

echo.
echo ----------------------------------------------------------------------------------
echo  Compile libdigitalscratch ...
echo ----------------------------------------------------------------------------------
echo.
cd ..\libdigitalscratch
if !errorlevel! neq 0 goto error
rmdir /S /Q release 2> NUL
if !errorlevel! neq 0 goto error
rmdir /S /Q debug 2> NUL
if !errorlevel! neq 0 goto error
qmake.exe
if !errorlevel! neq 0 goto error
nmake.exe -f Makefile.Release
if !errorlevel! neq 0 goto error

echo.
echo ----------------------------------------------------------------------------------
echo  Compile digitalscratch ...
echo ----------------------------------------------------------------------------------
echo.
cd ..\digitalscratch
if !errorlevel! neq 0 goto error
rmdir /S /Q release 2> NUL
if !errorlevel! neq 0 goto error
rmdir /S /Q debug 2> NUL
if !errorlevel! neq 0 goto error
qmake.exe
if !errorlevel! neq 0 goto error
nmake.exe -f Makefile.Release
if !errorlevel! neq 0 goto error
nmake.exe -f Makefile.Release clean
if !errorlevel! neq 0 goto error

GOTO end

:usage
echo.
echo USAGE: generate_digitalscratch_msi.bat [msi_name]
echo.
echo.

GOTO end

:error
echo.
echo FAILURE
exit /b %errorlevel% 
echo.

:end
echo.
echo ----------------------------------------------------------------------------------
echo  Done.
echo ----------------------------------------------------------------------------------
echo.
