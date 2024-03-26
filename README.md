<!--- @file
  Readme.md for UEFI / EDK II Training  Lab Material - Platform Build Lab FW 
  

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  Redistribution and use in source (original document form) and 'compiled'
  forms (converted to PDF, epub, HTML and other formats) with or without
  modification, are permitted provided that the following conditions are met:

  1) Redistributions of source code (original document form) must retain the
     above copyright notice, this list of conditions and the following
     disclaimer as the first lines of this file unmodified.

  2) Redistributions in compiled form (transformed to other DTDs, converted to
     PDF, epub, HTML and other formats) must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS DOCUMENTATION IS PROVIDED BY TIANOCORE PROJECT "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL TIANOCORE PROJECT  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS DOCUMENTATION, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-->

# PlatformBuildLab_FW

Lab Material for Platform Build Lab - Currently MinnowBoard Max/ Turbot

### PlatformBuildLab_FW will create a FW directory as follows:

- **FW**
  - PlatformBuildLab
     - Max source code for the Minnowboard Max / Turbot V 1.00


### Building for MinnowBoard Max / Turbot

1. Navigate to firmware sources directory:

    ```bash
    cd FW/PlatformBuildLad/Max
    ```

2. Place the TXE v1.1.4.1145 Tools in the respective directories:
    - TXEInfo and FPT for 32bit EFI in `silicon/Vlv2MiscBianies/Pkg/SEC/Ia32`
      as `TXEInfo.efi` and `fpt.efi` respectively. FPT also requires the
      `fparts.txt` file to be copied.
    - TXEInfo and FPT for 64bit EFI in `silicon/Vlv2MiscBianies/Pkg/SEC/X64`
      as `TXEInfo.efi` and `fpt.efi` respectively. FPT also requires the
      `fparts.txt` file to be copied.
3. Launch docker container:

    ```bash
    docker run --rm -it -w /home/edk2 -v $PWD:/home/edk2 3mdeb/edk2:1.1.0 /bin/bash
    ```

4. Inside the container navigate to `Vlv2TbltDevicePkg`:

   ```bash
   cd edk2-platforms/Vlv2TbltDevicePkg
   ```

5. Invoke the build:
    - For 16MB flash release build:

      ```bash
      ./Build_IFWI.sh /16M /Q /x64 MNW2 Release
      ```

    - For 8MB flash release build:

      ```bash
      ./Build_IFWI.sh /Q /x64 MNW2 Release
      ```

    - For 16MB flash debug build:

      ```bash
      ./Build_IFWI.sh /16M /Q /x64 MNW2 Debug
      ```

    - For 8MB flash debug build:

      ```bash
      ./Build_IFWI.sh /Q /x64 MNW2 Debug
      ```

    > Remove `/Q` flag to disabel quiet build to get verbose information about
    > errors, if encountered during build.
