:: This batch file automatically setups the development environment for Thetis
:: Note: RUN AS ADMINISTRATOR
@echo off

:: Step One: Install VSCode and git VCS
winget install -e --id Microsoft.VisualStudioCode
winget install --id Git.Git -e --source winget
winget install --id=GitHub.cli  -e

:: Step Two: Install extensions for VSCode
:: The PlatformIO IDE is used to compile and upload the code to the Thetis board
code --install-extension platformio.platformio-ide
:: CPP Tools will make development with CPP slightly easier
code --install-extension ms-vscode.cpptools

:: Step Three: Clone Thetis-Firmware Repository
setlocal

:: Get user input for where to clone the repository to
set "psCommand="(new-object -COM 'Shell.Application')^
.BrowseForFolder(0,'Please choose a folder to clone the Thetis firmware into.',0,0).self.path""

for /f "usebackq delims=" %%I in (`powershell %psCommand%`) do set "folder=%%I\Thetis-Firmware\"

setlocal enabledelayedexpansion
git clone https://github.com/Legohead259/Thetis-Firmware.git !folder!

chdir !folder!

:: Initialize all the git submodules in the repository
git submodule update --init --recursive
endlocal

:: Step Four: Check proper PIO and firmware installation by compiling
echo Checking that firmware compiles using PlatformIO... Please Standby...
%USERPROFILE%\.platformio\penv\Scripts\platformio.exe run -e thetis_revf5

set /p "pio_success=Did the firmware compile successfully (Y/N)? "

if "%pio_success%"=="Y" goto yes
if "%pio_success%"=="y" goto yes
goto no

:yes
echo Excellent! The development environment for Thetis is set up.
echo:
set /p "git_setup=Would you like to set up your Git repository now (you will need a GitHub account) (Y/N)? "

if "%git_setup%"=="Y" goto git_yes
if "%git_setup%"=="y" goto git_yes
goto git_no

:no
echo Hmmmmm. Something may have gone wrong. Consult the README and redo the steps manually.
EXIT \B 1

:git_yes
git_setup
echo Done! Thank you.
EXIT \B 0

:git_no
echo Okay. You may set up your git stuff later following the README. Thank you.
EXIT \B 0