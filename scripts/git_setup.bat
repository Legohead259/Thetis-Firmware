:: git_setup.bat
:: This script will initialize the git environment for the user.

@echo off

setlocal

SET /p "GIT_USER=Please enter your GitHub username: "
SET /p "GIT_EMAIL=Please enter your GitHub email address: "

:: Configure git username and email
git config --global user.name %GIT_USER%
git config --global user.email %GIT_EMAIL%

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
gh repo fork https://github.com/Legohead259/ThetisLib.git --remote=true
gh repo fork https://github.com/Legohead259/Fusion-Arduino.git --remote=true
gh repo fork https://github.com/Legohead259/Timer-Events-Arduino.git --remote=true
gh repo fork https://github.com/Legohead259/xioAPI-Arduino.git --remote=true

:: Edit the Thetis-Firmware submodules to point at the new fork URL
git submodule set-url lib/ThetisLib https://github.com/%GIT_USER%/ThetisLib.git
chdir lib\ThetisLib\
git submodule set-url src/fusion https://github.com/%GIT_USER%/Fusion-Arduino.git
git submodule set-url timing https://github.com/%GIT_USER%/Timer-Events-Arduino.git
git submodule set-url src/xioAPI https://github.com/%GIT_USER%/xioAPI-Arduino.git
git submodule sync
chdir ..\..\
git submodule sync

:no
echo Okay. You may set up your git stuff later following the README. Thank you.
EXIT \B 0