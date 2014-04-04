echo off
SETLOCAL EnableDelayedExpansion
REM
REM Tools for packaging DigitalScratch software as a MSI setup installation file.
REM

echo.
echo ----------------------------------------------------------------------------------
echo  Setup environment ...
echo ----------------------------------------------------------------------------------
echo.
set CURDIR=%CD%
set QTDIR=C:\Qt\5.2.0\msvc2010_opengl
call "%QTDIR%\bin\qtenv2.bat"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /x86
cd /D %CURDIR% 

echo QTDIR=%QTDIR%

echo.
echo ----------------------------------------------------------------------------------
echo  Get version from .pro ...
echo ----------------------------------------------------------------------------------
echo.
FOR /F "tokens=1-2 delims==" %%a IN ('FINDSTR /RC:"VERSION = " ..\digitalscratch\digitalscratch.pro ^| FINDSTR /VC:"#"') DO set VERSION=%%b
if !errorlevel! neq 0 goto error
set VERSION=%VERSION: =%
SET OUTPUT_MSI=digitalscratch-%VERSION%.msi
echo Creating %OUTPUT_MSI%...

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

echo.
echo ----------------------------------------------------------------------------------
echo  Prepare files ...
echo ----------------------------------------------------------------------------------
echo.
copy COPYING release
if !errorlevel! neq 0 goto error
copy README release
if !errorlevel! neq 0 goto error
copy AUTHORS release
if !errorlevel! neq 0 goto error
cd ..\windows
move /Y ..\digitalscratch\release\digitalscratch.exe .

echo.
echo ----------------------------------------------------------------------------------
echo  Generate %OUTPUT_MSI%...
echo ----------------------------------------------------------------------------------
echo.
del /Q /F %OUTPUT_MSI% 2> NUL
"C:\Program Files (x86)\WiX Toolset v3.8\bin\heat.exe" dir "..\digitalscratch\release" -srd -cg digitalscratchCG -gg -scom -sreg -sfrag -dr INSTALLDIR -out "digitalscratchFiles.wxsfrg" -var var.digitalscratchFiles
"C:\Program Files (x86)\WiX Toolset v3.8\bin\candle.exe" -dProductVersion=%VERSION% -ddigitalscratchFiles="..\digitalscratch\release" digitalscratchFiles.wxsfrg msi_config.wxs
"C:\Program Files (x86)\WiX Toolset v3.8\bin\light.exe" -sw -ext WixUIExtension -dWixUIBannerBmp="top_banner.bmp" -dWixUIDialogBmp="side_banner.bmp" -out %OUTPUT_MSI% digitalscratchFiles.wixobj msi_config.wixobj
del /Q /F *.wixobj 2> NUL
del /Q /F *.wixpdb 2> NUL
del /Q /F *.wxsfrg 2> NUL

IF NOT EXIST %OUTPUT_MSI% (
  echo.
  echo ERROR: MSI generation failed
  echo.
  exit /b -1
) ELSE (
  echo.
  echo MSI is OK.
  echo.
)

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
