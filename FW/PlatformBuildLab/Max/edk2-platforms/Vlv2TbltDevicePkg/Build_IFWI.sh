#!/usr/bin/env bash
##**********************************************************************
## Function define
##**********************************************************************
function Usage ( ) {
  echo
  echo "Script to build BIOS firmware and stitch the entire IFWI."
  echo
  echo "Usage: Build_IFWI.sh [options]  PlatformType  BuildTarget  "
  echo
  echo "  [options]"
  echo "       /c     CleanAll before building"
  echo "       /x64   Set Arch to X64  (default: X64)"
  echo "       /IA32  Set Arch to IA32 (default: X64)"
  echo "       /yL    Enable SPI lock for Stitch"
  echo "       /L     create a log "
  echo 
  echo " Required Parameters"
  echo "       Platform_Types:   MNW2               "
  echo "       Build_Targets:    Release, Debug     "
  echo
  echo "       See  Stitch/Stitch_Config.txt  for additional stitching settings."
  echo
  echo "Press Control-C to exit"
  read
  return 0
  #exit 0
}

## Assign initial values
exitCode=0
Build_Flags=
Stitch_Flags=
Arch=X64
PLATFORM_PACKAGE=Vlv2TbltDevicePkg
CleanBuild=0

## Parse Optional arguments
if [ "$1" == "/?" ]; then
  Usage
  exit 0
fi

for (( i=1; i<=$#; ))
  do
    if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/Q" ]; then
      Build_Flags="$Build_Flags /q"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/L" ]; then
      Build_Flags="$Build_Flags /l"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/C" ]; then
      Build_Flags="$Build_Flags /c"
      CleanBuild=1
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/ECP" ]; then
      Build_Flags="$Build_Flags /ecp"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/P3" ]; then
      Build_Flags="$Build_Flags /P3"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/P5" ]; then
      Build_Flags="$Build_Flags /P5"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/X64" ]; then
      Arch=X64
      Build_Flags="$Build_Flags /x64"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/IA32" ]; then
      Arch=IA32
      Build_Flags="$Build_Flags /IA32"
      shift
    elif [ "$1" == "/nG" ]; then
      Stitch_Flags="$Stitch_Flags /nG"
      shift
    elif [ "$1" == "/nM" ]; then
      Stitch_Flags="$Stitch_Flags /nM"
      shift
    elif [ "$1" == "/nB" ]; then
      Stitch_Flags="$Stitch_Flags /nB"
      shift
    elif [ "$1" == "/nV" ]; then
      Stitch_Flags="$Stitch_Flags /nV"
      shift
    elif [ "$1" == "/yL" ]; then
      Build_Flags="$Build_Flags /yL"
      shift
    elif [ "$1" == "/16M" ]; then
      Build_Flags="$Build_Flags /16M"
      shift
    else
      break
    fi
  done


## Require 2 input parameters
if [ "$2" == "" ]; then
  echo "Not Enough Arguments Provided"
  echo "Please review the Help screen"
  Usage
  echo "Exiting the Build"
  echo "press cnt-C or any key to quit this terminal session"
  read
  exit 0
fi
echo "else more than 2 input parameters"
## Assign required arguments
Platform_Type=$1
Build_Target=$2
if [ "$3" == "" ]; then
  IFWI_Suffix=
else
  IFWI_Suffix="/S $3"
fi

if [ $Arch == "X64" ]; then

  ## Build iPXE
  if [ ! -d ../../ipxe ]; then
    git clone http://git.ipxe.org/ipxe.git ../../ipxe
    if [ $? != 0 ];then
      echo "Failed to clone ipxe"
      exit
    fi
  fi

  if [ ! -f ../../ipxe/src/bin-x86_64-efi-sb/ipxe.efi ]; then
    cd ../../ipxe || exit 1
    git reset --hard HEAD || exit 1
    git clean -df || exit 1
    git checkout 77b07ea4fdc259d7253c6f9df2beda6e6c7a9d85 || exit 1
    if [ $CleanBuild == "1" ]; then
      make clean || exit 1
    fi
    sed -i 's|//#define\s*IMAGE_SCRIPT.*|#define IMAGE_SCRIPT|' "src/config/general.h"
    if [ ! -f dasharo.ipxe ]; then
      wget http://raw.githubusercontent.com/Dasharo/dasharo-blobs/main/dasharo/dasharo.ipxe || exit 1
    fi
    sed -i 's|.*DOWNLOAD_PROTO_HTTPS|#define DOWNLOAD_PROTO_HTTPS|g'  "src/config/general.h"
    make -C src bin-x86_64-efi-sb/ipxe.efi EMBED=$PWD/dasharo.ipxe BUILD_ID_CMD="echo 0x1234567890"
    if [ ! -f src/bin-x86_64-efi-sb/ipxe.efi ]; then
      echo "Failed to build ipxe"
      exit 1
    fi
    cd ../edk2-platforms/$PLATFORM_PACKAGE
  fi
fi

## Go to root directory
cd ..

## Build BIOS
echo "======================================================================"
echo "Build_IFWI:  Calling BIOS build Script..."
echo "Before call to ./$PLATFORM_PACKAGE/bld_vlv.sh $Build_Flags $Platform_Type $Build_Target"
./$PLATFORM_PACKAGE/bld_vlv.sh $Build_Flags $Platform_Type $Build_Target

echo
echo Finished Building BIOS.

## Start Integration process
echo ======================================================================
echo Skip "Build_IFWI:  Calling IFWI Stitching Script..."

echo
echo Build_IFWI is finished.
echo The final IFWI file is located in $PLATFORM_PACKAGE/Stitch
echo ======================================================================
