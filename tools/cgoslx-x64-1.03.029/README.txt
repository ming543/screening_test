congatec Linux CGOS Package
============================

This package contains the following components:

CgosDrv		: congatec CGOS kernel mode driver for Linux.
CgosLib		: congatec CGOS interface library for Linux.
CgosDump	: Simple CGOS tool to verify correct CGOS installation.
CgosMon		: Simple CGOS hardware monitoring utility.
CgosTest	: Test tool and code reference for accessing all CGOS functions.

Installation:
-------------

- For Ubuntu, run

  sudo apt install build-essential

  Since Ubuntu 18.04, the package libelf-dev has to be installed, too:

  sudo apt install libelf-dev

- For Fedora (tested on Fedora 27, 28 and 29), run

  sudo dnf install @development-tools elfutils-libelf-devel kernel-devel-$(uname -r)

- in the package root folder, compile the sources using 
  
  make

- after that, install with

  sudo make install
  
- If libcgos.so can not be found (e.g. on Fedora), you need to (re-)configure your dynamic linker
  run-time bindings by running 

  sudo ldconfig

- cgostest contains sample code for all CGOS functions. It is recommended to use it 
  for verifying functionality and as a reference for writing your own applications.

Notes:
------

When running make install, the udev rule 99-cgos.rules will be copied to
/lib/udev/rules.d if the file does not already exist there. It will set the permission
for accessing the CGOS driver to "0666" (r/w for everyone). You can change the permissions 
according to your needs either by modifying the rule file or by adding your custom user 
rule to /etc/udev/rules.d.

  permission to:  owner      group      other     
                  /¯¯¯\      /¯¯¯\      /¯¯¯\
  octal:            6          6          6
  binary:         1 1 0      1 1 0      1 1 0
  what to permit: r w x      r w x      r w x

  binary         - 1: enabled, 0: disabled

  what to permit - r: read, w: write, x: execute

  permission to  - owner: the user that created the file/folder
                   group: the users from the group where the owner is member
                   other: all other users

Additionally, a cgos.conf file will be copied to /usr/lib/modules-load.d
for having the cgosdrv kernel module loaded after each boot per default.


LICENSE:
--------
The congatec Linux CGOS package is free software.
You can redistribute it and/or modify it under the terms of the applicable
licenses that accompany this distribution.

The CGOS kernel mode driver (CgosDrv) ist distributed under the terms of the 
GNU General Public License version 2.
The CGOS interface library (CgosLib) and the included CGOS tools (CgosDump, CgosMon)
are distributed under the terms of the BSD 2-clause license.

Please refer to the license information in the source files and in the 
single COPYING_xxx file(s) for detailed information.


