#!/bin/bash

################################################################################
# Generate ubuntu packages: digitalscratch
################################################################################

# Error checking
function check_error {
    if [ $? -gt 0 ]; then
        echo "ERROR ! ABORTING !"
		exit
	fi
}

# Usage
function usage {
    echo ""
    echo "Usage: generate_digitalscratch_deb.sh [version_number] [ppa_type]"
    echo ""
    echo "    [version_number] is a 3 digits version like 1.1.0"
    echo "    [ppa_type]       'test' (for the test PPA url) or 'prod'"
    echo ""
    exit
}

# Check parameters
if [ $# -ne 2 ]; then
    usage
fi

# Main vars
VERSION=$1
VERSIONPACKAGE=$1-0ubuntu1
WORKINGPATH=$HOME/digitalscratch_$1-make_package
DEBPATH=$WORKINGPATH/deb
SOURCEDIR=digitalscratch_source
TARPACK=digitalscratch_$1.orig.tar.gz
ORIGDIR=$(pwd)
DISTRIB=$(lsb_release -cs)
PPAURL=http://ppa.launchpad.net
export DEBEMAIL=julien.rosener@digital-scratch.org
export DEBFULLNAME="Julien Rosener"
export EDITOR=vim

# Select PPA
if [[ $2 == test ]] ; then
    PPAPATH=julien-rosener/digitalscratch-test
elif [[ $2 == prod ]] ; then
    PPAPATH=julien-rosener/digitalscratch
else
    usage
fi

echo "****************************** Install tools ****************************"
sudo apt-get install packaging-dev build-essential dh-make
check_error
echo ""
echo ""

echo "*************************** Prepare environment *************************"
rm -rf $WORKINGPATH
check_error
mkdir -v $WORKINGPATH
check_error
echo ""
echo ""

echo "************************* Change version in .pro ************************"
sed -i "s/\(^VERSION = \)\(..*$\)/\1$VERSION/" ../digitalscratch.pro
check_error
cat ../digitalscratch.pro | grep 'VERSION ='
check_error
echo ""
echo ""

echo "**************************** Copy source code ***************************"
svn revert changelog
check_error
svn export ../ $WORKINGPATH/$SOURCEDIR
check_error
echo ""
echo ""

echo "************************* Update changelog ******************************"
ORIGDIR=$(pwd)
cd $WORKINGPATH/$SOURCEDIR
debchange --newversion $VERSIONPACKAGE --distribution $DISTRIB
check_error
cat $WORKINGPATH/$SOURCEDIR/debian/changelog
cp $WORKINGPATH/$SOURCEDIR/debian/changelog $ORIGDIR
check_error
echo ""
echo ""

echo "************************* Compress source directory *********************"
cd $WORKINGPATH
tar cvzf $TARPACK $SOURCEDIR/
echo ""
echo ""

echo "***************************** Create Linux base *************************"
export BUILDUSERID=$USER
cp $ORIGDIR/.pbuilderrc ~/
check_error
cd $WORKINGPATH/$SOURCEDIR
export OTHERMIRROR="deb $PPAURL/$PPAPATH/ubuntu $DISTRIB main"
pbuilder-dist $DISTRIB update
echo ""
echo ""

echo "************Parse debian/ config file and create source.changes *********"
debuild -S -sa
check_error
cd ../
echo ""
echo ""

echo "***************************** Create test DEB files *********************"
mkdir -v $DEBPATH
pbuilder-dist $DISTRIB build --allow-untrusted --buildresult $DEBPATH *.dsc
check_error
echo ""
echo ""

echo "************ Upload source.changes on Launchpad at $PPAPATH *************"
dput -f ppa:$PPAPATH *source.changes
check_error
echo ""
echo ""

echo "    Done, testing DEBs are in $DEBPATH"

