#!/bin/bash

################################################################################
# Generate Debian packages: digitalscratch
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
    echo "Usage: generate_digitalscratch_deb.sh [archi] [repo_type]"
    echo ""
    echo "    [archi]  'amd64' or 'i386'"
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
    REPOPATH=$(readlink -f ../../../../gh-pages/debian-test/)
    REPOURL=http://www.digital-scratch.org/debian-test/
elif [[ $2 == prod ]] ; then
    REPOPATH=$(readlink -f ../../../../gh-pages/debian/)
    REPOURL=http://www.digital-scratch.org/debian/
else
    usage
fi

echo "****************************** Install tools ****************************"
sudo apt-get install packaging-dev build-essential dh-make reprepro
check_error
echo ""
echo ""

echo "************************* Get version from .pro ************************"
VERSION=$(cat ../../digitalscratch.pro | grep -i '^\s*VERSION =' | grep 'CURRENT_DATE' | cut -d'=' -f2 | tr -d ' ' | tr -d '\r')
VERSION=${VERSION/\$\$\{CURRENT_DATE\}/$(date +%Y%m%d)}
echo VERSION = $VERSION
check_error
echo ""
echo ""

echo "*************************** Prepare environment *************************"
# Main vars
VERSIONPACKAGE=$VERSION-1
WORKINGPATH=$HOME/digitalscratch_$VERSION-make_package
SOURCEDIR=digitalscratch_source
TARPACK=digitalscratch_$VERSION.orig.tar.gz
ORIGDIR=$(pwd)
DISTRIB=stable
DEBBIN=digitalscratch_${VERSIONPACKAGE}_${ARCHI}.deb
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
git archive --format zip --output $WORKINGPATH/archive.zip $(git symbolic-ref --short -q HEAD)
unzip $WORKINGPATH/archive.zip -d $WORKINGPATH/$SOURCEDIR
check_error
echo ""
echo ""

echo "**************************** Install debian/ folder ***************************"
cp -r $WORKINGPATH/$SOURCEDIR/dist/debian/debian $WORKINGPATH/$SOURCEDIR/
check_error
echo ""
echo ""

echo "************************* Update changelog ******************************"
cd dist/debian/debian
cd $WORKINGPATH/$SOURCEDIR
debchange --newversion $VERSIONPACKAGE --distribution $DISTRIB
check_error
cat $WORKINGPATH/$SOURCEDIR/debian/changelog
cp $WORKINGPATH/$SOURCEDIR/debian/changelog $ORIGDIR/debian
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
sudo pbuilder --create --architecture $ARCHI --distribution $DISTRIB --othermirror "deb [trusted=yes] $REPOURL stable main" --allow-untrusted
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
echo "$WORKINGPATH/${DEBBIN}"
dpkg -c $WORKINGPATH/${DEBBIN}
check_error
cd ../
echo ""
echo ""

echo "************ Install package to local apt repo $REPOPATH *************"
cd $ORIGDIR
cd $REPOPATH
#reprepro -A $ARCHI remove $DISTRIB digitalscratch
reprepro --ask-passphrase -Vb . includedeb stable $WORKINGPATH/${DEBBIN}
check_error
cd $ORIGDIR
echo ""
echo ""

echo "    Done."

