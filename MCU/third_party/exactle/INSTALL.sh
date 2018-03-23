#!/bin/bash
# INSTALL.sh: ExactLE installation script for Linux.
# Copyright (C) 2016 by Rick Foos <rfoos at ambiqmicro.com>
# Released under the BSD New license.
#

# Variables and Defaults.
[ -n "${DESTDIR}" ] || DESTDIR=~/AmbiqMicro/AmbiqSuite/third_party
# Not silent install is default.
[ -n "${SILENTINSTALL}" ] || SILENTINSTALL=""

PACKAGE_NAME="ExactLE"
PACKAGE_VERSION="AMBIQ CUSTOMER"
PACKAGE_BUGREPORT="support@ambiqmicro.com"
PACKAGE_STRING="${PACKAGE_NAME} ${PACKAGE_VERSION}"

# Messages
msg_Title="Installing ${PACKAGE_STRING} for all users to ${DESTDIR}"
msg_Contact_Support="\nIf you are having problems, contact us at ${PACKAGE_BUGREPORT}.\n"
msg_Please_Answer="\nPlease answer y, n, or d."
read -r -d '' msg_Silentinstall << EOM
\nSelect Yes to install to ${DESTDIR}.
Select No to manually select install options.\n
EOM
read -r -d '' msg_Versiondir << EOM
\nAnswer Yes to install to ${DESTDIR}/exactle/<version>.
Multiple versions can be installed using version.
\nAnswer No to install to ${DESTDIR}/exactle.
Any previous version will be replaced.
EOM

# Commands
ECHO_E="/bin/echo -e"
MKDIR_P="/bin/mkdir -p"
CP_AP="/bin/cp --preserve=mode,timestamps"
RM_F="/bin/rm -f"
RM_RF="/bin/rm -rf"

# Functions
function echoe()   { ${ECHO_E} "$@"; }
function echoerr() { ${ECHO_E} "$@\n" 1>&2; }
function exiterr() {
[ -n "$@" ] && echoerr "$@"
echoerr "${msg_Contact_Support}"
exit 1
}
function mkdirp ()  { ${ECHO_E} "Making directory $@";${MKDIR_P} "$@"; }
function cpap ()    { ${ECHO_E} "Copying files $@";${CP_AP} "$@"; }
function rmf()    { ${ECHO_E} "Removing files $@";${RM_F} "$@"; }
function rmrf()    { ${ECHO_E} "Removing directories $@";${RM_RF} "$@"; }

function mkdirpe ()  { mkdirp $@;[ $? -gt 0 ] && exiterr "ERROR: $? in mkdir"; }
function cpape ()    { cpap $@;[ $? -gt 0 ] && exiterr "ERROR: $? in cp"; }
function rmfe ()    { rmf $@;[ $? -gt 0 ] && exiterr "ERROR: $? in rm -f"; }
function rmrfe ()    { rmrf $@;[ $? -gt 0 ] && exiterr "ERROR: $? in rm -rf"; }

# Start
echoerr "${msg_Title}"

# Make sure we are inside an install or source directory.
# find LICENSE, and exactle*.tgz

# Check installation, and source(dev only) directories.
if [ -f LICENSE ] && [ -f INSTALL.sh ] && [ -d docs ]
then
    echoerr "\nInstalling from distribution directory."
else
    exiterr "\nCannot run INSTALL outside of distribution directory. Exiting."
fi

# If the caller hasn't specified Silent Install, ask.
if [ -z "${SILENTINSTALL}" ]
then
    while true; do
    read -p "Would you like a silent install(y|n|d)?" yn
    case $yn in
        [Yy]* ) SILENTINSTALL="YES"; break;;
        [Nn]* ) SILENTINSTALL=""; break;;
        [Dd]* ) echoerr "${msg_Silentinstall}";;
        * ) echoerr "${msg_Please_Answer}";;
    esac
    done
fi

echoerr "Installing ${PACKAGE_NAME}"

[ -f LICENSE ] || exiterr "Click-wrap license not included with package."
# Click-wrap License
echoerr "License must be acknowledged to install."
echoerr "Please review license, press 'q' to return."
read -p "Press any key to review License."
(less LICENSE)
while true; do
read -p "Press Y agree to the License (y|n|d)?" yn
case $yn in
    [Yy]* ) break;;
    [Nn]* ) exiterr "\nNot installing ${PACKAGE_NAME} at this time.";;
    [Dd]* ) echoerr "\nAnswer Yes if you agree with License and want to install ${PACKAGE_NAME}.";;
    * )     echoerr "${msg_Please_Answer}";;
esac
done

DESTDIR=${DESTDIR}

# Check for previous installation.
if [ -d ${DESTDIR}/exactle ]
then
    echoerr "${PACKAGE_NAME} ${DESTDIR}/exactle already installed."
    if [ -z "${SILENTINSTALL}" ]
    then
    while true; do
    read -p "Do you want to install anyway (y|n|d)?" yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exiterr "\nNot installing ${PACKAGE_NAME} at this time.";;
        [Dd]* ) echoerr "\nAnswer Yes to replace existing ${PACKAGE_NAME}  installation.";;
        * )     echoerr "${msg_Please_Answer}";;
    esac
    done
    # Install anyway.
    #else exit 1;
    fi
fi

# Ask for destination directory.
if [ -z "${SILENTINSTALL}" ]
then
    while true; do
    read -p "Would you like to install ${PACKAGE_NAME} to ${DESTDIR}(y|n|d)?" yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) answerno="true"; break;;
        [Dd]* ) echoerr "Answer Yes to continue.\nAnswer No to change installation directory.";;
        * ) echoerr "${msg_Please_Answer}";;
    esac
    done
    if [ -n "${answerno}" ]
    then
        read -p "Enter ${PACKAGE_NAME} install directory(${DESTDIR})?" installdir
        [ -n "${installdir}" ] && DESTDIR=${installdir}
    fi
fi

echoerr "Installing ${PACKAGE_NAME} to ${DESTDIR}."

# copy files.
rmrf ${DESTDIR}/exactle
mkdirpe ${DESTDIR}/exactle
cpape -r * ${DESTDIR}/exactle
rmfe ${DESTDIR}/exactle/INSTALL.sh
#tar xf exactle*.tar.gz -C ${DESTDIR}

echoerr "\n${PACKAGE_NAME} Installed to: ${DESTDIR}"
# Display version info

echoerr "\nDone!";
