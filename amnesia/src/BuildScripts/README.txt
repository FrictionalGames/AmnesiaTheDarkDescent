How to build on linux
---------------------

Summary
=======
Currently this build system is centered around mock.   mock is a build system used for Fedora, CentOS, and RHEL to build packages.  The way it works is it installed a "chroot" (a separate root) with the needed build environment to compile the package.  This provides a clean, consistent, and automated way of building software.  And because we are "piggybacking" off of CentOS for our build roots we gain stability and long-term support of the build environment.

The scripts in this directory will prepare a mock environment based on CentOS 6 (32bit and 64bit when Steam supports it).  There is a certain order to run things.  and some steps only need to be done the first time.

Games build in this system have proven very stable and VERY portable across a wide variety of distributions.

Initial Setup
=============

mock-prep.sh must be run initially to prepare the build root..  This sets up the core mock build environment and installs a couple of additional packages needed to build Amnesia: AMFP (or pretty much any game). Simply run this command to set it up

./mock-prep.sh 32 init

- 32 is the arch to setup, valid options are 32 or 64
- init simply says "remove any existing root and start from scratch"

Syncing the code
================

Since it is a separate environment that will be "chrooted" into (meaning it won't have access to the host system) we must either "mount" the code in the root or copy it into the root.  There is a file named mock-sync.txt which defines the folders to copy in to build the code.  And the script mock-sync.sh does the work for us.

./mock-sync.sh 32

- again 32 is the arch.. 64 is also valid

Preparing for building the code
===============================

(this is about manually setting things up to build.. there is a build-linux.sh script that does all this for you)

The initial setup of building the code is to setup the build dirs via cmake so we must shell into the chroot.

./mock-shell 32

then we go into the synced code directory

cd builddir/AmnesiaPig

Now create our build folders and setup cmake

mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DUSE_SDL2=ON
cd ..

now we can exit the chroot.

exit

You'll see some output like this

[mockbuild@LinuxBoxen build-setup]$ exit
logout
Finish: shell
Finish: lock buildroot

Building the code
=================

You can either build the code in the chroot manually by running "make" in the build-game or build-setup directories. 
(use -j# to speed up the build by doing simultaneous compiles.. e.g.  make -j4)

To more automate the build process I have included a "build-linux.sh" script that will build the game and extract out the binaries for you into the Release folder.

