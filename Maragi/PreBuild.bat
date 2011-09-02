@echo off

rem $Id$

if "%1" == "" goto end
if not exist %1\Tokens.txt goto create
goto process

:create
echo > %1\Tokens.txt

:process
echo Merging Tokens.h.in and Tokens.txt into Tokens.h...
perl %1\TextReplacer.pl %1\Tokens.h.in %1\Tokens.txt > %1\Tokens.h

:end