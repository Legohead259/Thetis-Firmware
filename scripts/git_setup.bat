:: git_setup.bat
:: This script will initialize the git environment for the user.

@echo off

setlocal

SET /p "GIT_USER=Please enter your GitHub username: "
SET /p "GIT_EMAIL=Please enter your GitHub email address: "

:: Configure git username and email
git config user.name %GIT_USER%
git config user.email %GIT_EMAIL%

git config user.name
git config user.email

:: Authorize GitHub CLI using your credentials
gh auth login -p "https" -w

:: Fork repository
echo FOR DEVELOPERS you need to fork the repository.
echo Are you a developer and do you want to add a feature to the firmware?
set /p "FORK=Do you want to fork the repository now (Y/N)? "

if "%FORK%" == "y" goto yes
if "%FORK%" == "Y" goto yes
goto no

:yes
chdir ..\
gh repo fork --remote=true

:no
echo Okay. You may set up your git stuff later following the README. Thank you.
EXIT \B 0