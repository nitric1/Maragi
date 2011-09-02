rem $Id$
@echo off
echo Merging Tokens.h.in and Tokens.txt into Tokens.h...
perl %1 %2 %3 > %4