echo off
SETLOCAL EnableDelayedExpansion
REM
REM Tools for packaging DigitalScratch software as a setup installation file.
REM

REM
REM Check parameters
REM
IF [%2]==[] (
  echo ERROR: Incorrect number of mandatory parameters !
  GOTO usage
)

REM
REM Get params
REM
SET VERSION=%1
SET OUTPUT_MSI=%2


echo.
echo ----------------------------------------------------------------------------------
echo  Change version in .pro ...
echo ----------------------------------------------------------------------------------
echo.
powershell -command "(Get-Content ..\digitalscratch\digitalscratch.pro) | ForEach-Object { $_ -replace '^VERSION = .+$', 'VERSION = %VERSION%' } | Set-Content ..\digitalscratch\digitalscratch.pro"
powershell -command "(Get-Content ..\libdigitalscratch\libdigitalscratch.pro) | ForEach-Object { $_ -replace '^VERSION = .+$', 'VERSION = %VERSION%' } | Set-Content ..\libdigitalscratch\libdigitalscratch.pro"

echo.
echo ----------------------------------------------------------------------------------
echo  Setup environment ...
echo ----------------------------------------------------------------------------------
echo.
call "C:\Qt\4.8.4\bin\qtvars.bat"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /x86

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
"C:\Program Files (x86)\WiX Toolset v3.7\bin\heat.exe" dir "..\digitalscratch\release" -srd -cg digitalscratchCG -gg -scom -sreg -sfrag -dr INSTALLDIR -out "digitalscratchFiles.wxsfrg" -var var.digitalscratchFiles
"C:\Program Files (x86)\WiX Toolset v3.7\bin\candle.exe" -dProductVersion=%VERSION% -ddigitalscratchFiles="..\digitalscratch\release" digitalscratchFiles.wxsfrg msi_config.wxs
"C:\Program Files (x86)\WiX Toolset v3.7\bin\light.exe" -sw -ext WixUIExtension -dWixUIBannerBmp="top_banner.bmp" -dWixUIDialogBmp="side_banner.bmp" -out %OUTPUT_MSI% digitalscratchFiles.wixobj msi_config.wixobj
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

:usage
echo.
echo USAGE: generate_digitalscratch_msi.bat [version] [msi_name]
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
