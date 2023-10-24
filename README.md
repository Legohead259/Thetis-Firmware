# Thetis-Firmware

The Thetis instrumentation board is an open source all-in-one inertial data logger for three dimensional bodies.
It was designed and developed for a Master's thesis in Ocean Engineering and the Florida Institute of Technology from 2020 to 2023.
This repository is the firmware used on that board to capture sensor measurements and compute body orientation and log all the appropriate data.

## Installing the Development Environment
This firmware was developed in Microsoft Visual Studio Code using the PlatformIO extension on Windows 10 for compilation and uploading.
To automatically install and initialize the environment, download the scripts in [scripts/](scripts/) and execute `1-dev_environment_setup.bat` as an administrator, following the on-screen prompts.
It will go through the steps discussed in the following sections.

### Step 1: Install VS Code and git VCS
[Download VisualStudio Code](https://code.visualstudio.com/download) from Microsoft and install it following the on-screen prompts.
Alternatively, it can be installed from the command prompt as an administrative user via:

```
winget install -e -id Microsoft.VisualStudioCode
```

Then, [download git](https://git-scm.com/downloads) and install it using the on-screen prompts.
You should keep everything to the default value except for the text editor.
I would recommend Nano over Vim (unless you know how to exit Vim).
This can also be installed from the command prompt via:

```
winget install -id Git.Git -e --source winget
```

Finally, **for developers only** [download the GitHub CLI client](https://cli.github.com/) and install it.
This can be done from the command prompt via:

```
winget install --id=GitHub.cli -e
```

Close and reopen any command prompt windows and test your installations with the following command:

```
code --version && git  --version && gh --version
```

If all the software has been installed correctly, you should see a short output similar to this:

```
1.83.1
f1b07bd25dfad64b0167beb15359ae573aecd2cc
x64
git version 2.42.0.windows.2
gh version 2.36.0 (2023-10-03)
https://github.com/cli/cli/releases/tag/v2.36.0
```

### Step Two: Install Extensions for VSCode
If all the software has been installed correctly, we can now install the required extensions.[PlatformIO](https://platformio.org/) is an advanced development environment for embedded systems.
It has excellent integration with VSCode that allows for large projects to be more easily managed and programmed.
To install PlatformIO through the VSCode GUI, open the application and look for the "Extensions" tab on the left-hand side (in red).
Select it, then search for PlatformIO in the marketplace (green).
Do the same for CPP Tools (blue) if not already installed.

![vscode_extension_platformio](https://github.com/Legohead259/Thetis-Firmware/assets/8077426/b968ce42-4609-4e99-baf6-2b7141576d35)
![vscode_extension_cpptools](https://github.com/Legohead259/Thetis-Firmware/assets/8077426/0227449c-285a-4520-aa28-f5f537a7c062)

Alternatively, these can be installed in the command prompt with:

```
code --install-extension platformio.platformio-ide
code --install-extension ms-vscode.cpptools
```

### Step Three: Clone the Thetis-Firmware Repository to a Directory
Once the applications have been installed, you can clone the [Thetis-Firmware repository](https://github.com/Legohead259/Thetis-Firmware) to your local machine.
Navigate to a directory you want to install the firmware onto (e.g. "C:\USER\Desktop\Thetis\") and open that location in a command prompt.
(*Hint:*: You can right-click within the folder and select "Open Git Bash Here" option.)
Then, execute the command:

```
git clone https://github.com/Legohead259/Thetis-Firmware.git
```
![file_explorer_git_bash](https://github.com/Legohead259/Thetis-Firmware/assets/8077426/49f97793-3280-4638-8a36-77eaf7cc4e58)

Once the folder is cloned, open it in the command prompt and initialize the submodule repositories with the command:

```
chdir Thetis-Firmware && git submodule update --init --recursive
```

### Step Four: Check Proper PIO and Firmware Installation by Compiling
Once the previous command finishes, exectute the following command to compile the firmware and ensure everything is installed properly.

```
%USERPROFILE%\.platformio\penv\Scripts\platformio.exe run -e thetis_revf5
```

If the code successfully compiles, you should see an output like this:
![platformio_compile_test](https://github.com/Legohead259/Thetis-Firmware/assets/8077426/6332791e-fccd-4c3a-838b-6a45ecee166e)

If the command prompt shows that output, congratulations!
You have successfully installed the basic development environment to upload new code to Thetis.

## Advanced Development Environment Setup
**FOR DEVELOPERS ONLY**
Now that the development environment is set up, you must fork the firmware repositories to your own GitHub account.
This will preserve the original source code and allow you to use GitHub to track any changes you make.
Substantial changes can be made to the source repository through an appropriate [pull request](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request-from-a-fork).

To automatically configure the git environment, you can download and execute the [git_setup.bat](scripts/git_setup.bat) script.
Or, you can follow along with the upcoming sections.

### Configure Git Parameters
Open a new command prompt in the cloned `Thetis-Firmware/` folder.
Before starting anything, we need to tell git CLI who you are.
First, we call the following commands, replacing the `GITHUB_USER` and `GITHUB_EMAIL` with those for your GitHub account:

```
git config --global user.name GITHUB_USER
git config --global user.email GITHUB_EMAIL
```

Then, we will authorize the GitHub CLI client to make requests to the GitHub servers on your behalf:

```
gh auth login -p "https" -w
```

This will give you a one-time passcode and ask to open a web browser to log into GitHub.

### Fork Repositories and Reconfigure Submodules
Once you authorize GitHub CLI to use your account, we can begin forking the firmware repositories.
First, execute the commands:

```
gh repo fork https://github.com/Legohead259/Thetis-Firmware.git --remote=true
gh repo fork https://github.com/Legohead259/ThetisLib.git --remote=true
gh repo fork https://github.com/Legohead259/Fusion-Arduino.git --remote=true
gh repo fork https://github.com/Legohead259/Timer-Events-Arduino.git --remote=true
gh repo fork https://github.com/Legohead259/xioAPI-Arduino.git --remote=true
```

Then, we must redirect the submodule URLs to the new repositories.
Navigate to the `Thetis-Firmware/` root directory and call:

```
git submodule set-url lib/ThetisLib https://github.com/GITHUB_USER/ThetisLib.git
```

Again, replacing `GITHUB_USER` with your GitHub username.
Then, navigate to the `Thetis-Firmware/lib/ThetisLib/` folder and execute the following:

```
git submodule set-url src/fusion https://github.com/GITHUB_USER/Fusion-Arduino.git
git submodule set-url timing https://github.com/GITHUB_USER/Timer-Events-Arduino.git
git submodule set-url src/xioAPI https://github.com/GITHUB_USER/xioAPI-Arduino.git
git submodule sync
```

Return to the `Thetis-Firmware/` root directory and execute the submodule sync command again:

```
git submodule sync
```

This will redirect all submodule commands to your newly forked repositories and will allow you to amke changes to any part of the firmware.
You should now have your own dedicated git environment to modify Thetis and save those changes.
