#!/bin/bash

################################################################################
# Generate Debian packages: libdigitalscratch
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
    echo "Usage: generate_libdigitalscratch_deb.sh [archi] [repo_type]"
    echo ""
    echo "    [archi]     'amd64' or 'i386'"
    echo "    [repo_type] 'test' or 'prod'"
    echo ""
    exit
}

# Check parameters
if [ $# -ne 2 ]; then
    usage
fi

# Get archi
if [[ $1 == amd64 ]] ; then
    ARCHI=amd64
elif [[ $1 == i386 ]] ; then
    ARCHI=i386
else
    usage
fi

# Get repo
if [[ $2 == test ]] ; then
    REPOPATH=$(readlink -f ../../../../digitalscratch.gh-pages.git/debian-test/)
elif [[ $2 == prod ]] ; then
    REPOPATH=$(readlink -f ../../../../digitalscratch.gh-pages.git/debian/)
else
    usage
fi

echo "****************************** Install tools ****************************"
sudo apt-get install packaging-dev build-essential dh-make reprepro
check_error
echo ""
echo ""

echo "************************* Get version from .pro ************************"
VERSION=$(cat ../../libdigitalscratch.pro | grep -i '^VERSION ='| cut -d'=' -f2 | tr -d ' ' | tr -d '\r' | cut -d'+' -f1)
if [[ $1 == test ]] ; then
    VERSION=$VERSION+SNAPSHOT$(date +%Y%m%d)
    sed -i s/^VERSION\ =.*/VERSION\ =\ $VERSION/g ../../libdigitalscratch.pro
fi
echo VERSION = $VERSION
check_error
echo ""
echo ""

echo "*************************** Prepare environment *************************"
# Main vars
VERSIONPACKAGE=$VERSION-1
WORKINGPATH=$HOME/libdigitalscratch_$VERSION-make_package
SOURCEDIR=libdigitalscratch_source
TARPACK=libdigitalscratch_$VERSION.orig.tar.gz
ORIGDIR=$(pwd)
DISTRIB=stable
DEBBIN=libdigitalscratch1_${VERSIONPACKAGE}_${ARCHI}.deb
DEBDEV=libdigitalscratch-dev_${VERSIONPACKAGE}_${ARCHI}.deb
export DEBEMAIL=julien.rosener@digital-scratch.org
export DEBFULLNAME="Julien Rosener"
export EDITOR=vim

rm -rf $WORKINGPATH
check_error
mkdir -v $WORKINGPATH
check_error
echo ""
echo ""

echo "**************************** Copy source code ***************************"
git checkout debian/changelog
check_error
cd ../../
git ls-files | grep -v dist/ | grep -v test/ | tar -czf $WORKINGPATH/$TARPACK -T -
ORIGDIR=$(pwd)
mkdir -p $WORKINGPATH/$SOURCEDIR
cd $WORKINGPATH/$SOURCEDIR
tar -xzf $WORKINGPATH/$TARPACK
cd $ORIGDIR
check_error
echo ""
echo ""

echo "**************************** Install debian/ folder ***************************"
cp -r dist/debian/debian $WORKINGPATH/$SOURCEDIR/
check_error
echo ""
echo ""

echo "************************* Update changelog ******************************"
cd dist/debian/debian
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
cd $WORKINGPATH/$SOURCEDIR
sudo pbuilder --clean
sudo pbuilder --create --architecture $ARCHI --distribution $DISTRIB
echo ""
echo ""

echo "************Parse debian/ config file and create .debs *********"
pdebuild --architecture $ARCHI
check_error
cd ../
echo ""
echo ""

echo "************ Show content of packages *********"
cp /var/cache/pbuilder/result/${DEBBIN} $WORKINGPATH
cp /var/cache/pbuilder/result/${DEBDEV} $WORKINGPATH
echo "$WORKINGPATH/${DEBBIN}"
dpkg -c $WORKINGPATH/${DEBBIN}
echo ""
echo "$WORKINGPATH/${DEBDEV}"
dpkg -c $WORKINGPATH/${DEBDEV}
check_error
cd ../
echo ""
echo ""

echo "************ Install package to local apt repo $REPOPATH *************"
cd $REPOPATH
reprepro -A $ARCHI remove $DISTRIB libdigitalscratch1
reprepro -A $ARCHI remove $DISTRIB libdigitalscratch-dev
reprepro --ask-passphrase -Vb . includedeb stable $WORKINGPATH/${DEBBIN}
reprepro --ask-passphrase -Vb . includedeb stable $WORKINGPATH/${DEBDEV}
check_error
echo ""
echo ""

echo "    Done."

