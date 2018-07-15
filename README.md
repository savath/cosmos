# Installer script for COSMOS. #

If you want to integrate COSMOS into your software or contribute to the software you are considered a 'developer'.
If you just want to install COSMOS and you are not necessarily thinking of changing the software then you're considered a 'regular user'.

This installer script will help you to download and setup and install the essential elements of COSMOS: core, nodes, resources. This setup script only works on Linux and Mac. For Windows please follow the instructions from https://bitbucket.org/cosmos/core/wiki/install/windows

Before starting make sure you have the required tools installed:

* Linux prerequisites: git, cmake
* These should both already be installed. To verify, open a terminal window, type git, and press enter. If it is properly installed, a list of available commands and usage will appear. Repeat by typing cmake and verifying. 

* Mac prerequisites: Command Line Tools (or Xcode), git, cmake

If you do not have the required tools installed, please click on either [Mac](https://bitbucket.org/cosmos-project/core/wiki/install/mac) or [Linux](https://bitbucket.org/cosmos-project/core/wiki/install/linux_ubuntu) to
get the required tools before you proceed.


# 1. clone the installer script #

By default cosmos installs in the home folder of the current user. Open a terminal window and make sure you are in your home folder* (see bottom note) by entering the command
```
#!shell
cd
```
Clone the installer repository into the 'cosmos' folder. Enter the command
```
#!shell
git clone https://bitbucket.org/cosmos-project/installer.git cosmos
```
**note**: this command will create a 'cosmos' folder in your home directory and will copy the installer scripts.

# 2. Run cosmos-setup.sh #

**For Regular Users**

Open a terminal window, change the directory to the cosmos folder, and run the setup script by using the following two commands.
```
#!shell
cd cosmos
./cosmos-setup.sh
```
The setup script will download the basic repositories, build the cosmos/core software and install the binaries in the 'bin' folder. This step will take a few minutes depending on your computer (typically 2 min.).

**For Developers**

You will need to create and add an [RSA](https://confluence.atlassian.com/bitbucket/set-up-ssh-for-git-728138079.html) key to your bitbucket account. Follow the instructions at the [RSA](https://confluence.atlassian.com/bitbucket/set-up-ssh-for-git-728138079.html) link, or type the following commands in a terminal window.


```
#!shell
ssh-keygen
cd
cd .ssh/
ls
cat id_rsa.pub
(if you left the default file name as id_rsa, otherwise replace it with what you named it.)
```

Run the script with the 'developer' argument

(Note: If you already ran the setup script without the 'developer' argument, you must first delete the folders "source" "resources" and "nodes" in the cosmos folder, and then run the developer script.

```
#!shell
cd
cd cosmos
./cosmos-setup.sh developer
```
The developer argument sets the required variables inside the script, check the [options section](#markdown-header-options) to learn more about this.


# 3. Verify installation

check that the bin folder was created inside the 'cosmos' folder by entering the 'ls' command

```
#!shell
ls
```
you should be able to see a list of files and folders 
```
#!shell
bin              Dockerfile  lib    README.md  scripts  tmp
cosmos-setup.sh  include     nodes  resources  src

```

If the bin folder is not listed check the tmp/cmake.log file to find the reason (possibly there was a problem running cmake). See the [troubleshooting section](#markdown-header-troubleshooting).

finally, open a new terminal window to load the cosmos/bin path into your environment. Execute the cosmos agent command

```
#!shell
agent
```

if you see this line out of the agent command then you're set
```
#!shell
Usage: agent [ list | dump [soh, beat, ###] | node_name agent_name "request [ arguments ]" ]
```
COSMOS/core has been installed in your linux/macOS box.


# 4. Using COSMOS #

For developers we recommend installing Qt Creator and follow the relevant instructions on 

* COSMOS general setup instructions:[Windows](https://bitbucket.org/cosmos-project/core/wiki/install/windows) [Mac](https://bitbucket.org/cosmos-project/core/wiki/install/mac) [Linux (Ubuntu)](https://bitbucket.org/cosmos-project/core/wiki/install/linux_ubuntu)
* COSMOS/core setup instructions: https://bitbucket.org/cosmos-project/core
* API: http://cosmos-project.org/docs/core/current/

for users we recommend reading the API and tutorials 

* http://cosmos-project.org/docs/core/current/

The documentation is work in progress so if you have questions please contact us at cosmos@hsfl.hawaii.edu

-----------------------
# Options #

**Optional Step 2** : cosmos-setup.sh file variables

open the cosmos-setup.sh file with your favorite editor (vi, nano, Sublime Text, etc.) and change the configuration parameters at the head of the file. You must have the ssh keys configured between your PC and bitbucket account to be able to clone the software - developers clone the repository using ssh by default. These are the recommended installer script settings:
```
#!shell

developer='yes' 
verbose='yes'
cosmosFolder=~/cosmos
cosmosBuild='yes'
downloadRepositories='yes'
dockerBuild='no'
dockerRun='no'
```

Make sure to save the file. If you set the 'dockerBuild' or 'dockerRun' to 'yes' you must have 'docker' installed and you will also be asked to put your linux user password (sudo is executed). On the macOS if you don't have cmake installed it will install Homebrew, which will ask you to enter your computer user password.

----------------

# Troubleshooting #

**bin folder is missing**
check the tmp/cmake.log file to find the reason (possibly there was a problem running cmake). 

```
#!shell
cat tmp/cmake.log
```

If you encounter the following error
```
#!shell
Tell CMake where to find the compiler by setting either the environment   variable "CXX"
```
You may not have the correct comiler installed and are using gcc instead of g++ for C++. In a terminal window type
```
#!shell
g++
```
If it is not currently installed, type
```
#!shell
sudo apt install g++
```

check if the $PATH is set to the cosmos/bin folder (if not you may need to open a new terminal window for the $PATH to update, or $ source ~/.bash_profile)

```
#!shell
echo $PATH
```

output will look like
```
#!shell
/home/<your-user>/cosmos/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
```

*home folder on:

* mac: /Users/your_user/cosmos
* linux: /home/your_user/cosmos
* windows: C:/cosmos