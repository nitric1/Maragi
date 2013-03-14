@echo off
setlocal

rem $Id: PreBuild.bat 42 2012-08-04 18:23:44Z wdlee91 $

if "%1" == "" goto end

set PreBuildPath=###%1###
set PreBuildPath=%PreBuildPath:"###=%
set PreBuildPath=%PreBuildPath:###"=%
set PreBuildPath=%PreBuildPath:###=%

if not exist %PreBuildPath%\Tokens.txt goto create
goto process

:create
echo Creating empty Tokens.txt...
echo $1= > "%PreBuildPath%\Tokens.txt"
echo $2= >> "%PreBuildPath%\Tokens.txt"

:process
echo Merging Tokens.h.in and Tokens.txt into Tokens.h...
perl "%PreBuildPath%\TextReplacer.pl" "%PreBuildPath%\Tokens.h.in" "%PreBuildPath%\Tokens.txt" > "%PreBuildPath%\Tokens.h"

:end