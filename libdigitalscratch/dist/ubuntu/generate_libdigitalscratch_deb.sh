#!/bin/bash

################################################################################
# Generate ubuntu packages: libdigitalscratch
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
    echo "Usage: generate_libdigitalscratch_deb.sh [ppa_type] <--rebuild>"
    echo ""
    echo " Mandatory args:"
    echo "    [ppa_type]  'test' (for the test PPA url) or 'prod'"
    echo ""
    echo " Optional args:"
    echo "    <--rebuild> <path_to_orig.tar.gz> do not use the actual source code but the defined .orig.tar.gz"
    echo ""
    exit
}

# Check parameters
if (( $# < 1 )); then
    usage
fi

if (( $# == 2 )); then
    usage
fi

# Select PPA
if [[ $1 == test ]] ; then
    PPAPATH=julien-rosener/digitalscratch-test
elif [[ $1 == prod ]] ; then
    PPAPATH=julien-rosener/digitalscratch
else
    usage
fi

echo "****************************** Install tools ****************************"
sudo apt-get install packaging-dev build-essential dh-make
check_error
echo ""
echo ""

echo "************************* Get version from .pro ************************"
VERSION=$(cat ../../libdigitalscratch.pro | grep -i '^VERSION =' | cut -d'=' -f2 | tr -d ' ')
VERSION=${VERSION/\$\$\{CURRENT_DATE\}/$(date +%Y%m%d)}
echo VERSION = $VERSION
check_error
echo ""
echo ""

echo "*************************** Prepare environment *************************"
# Main vars
WORKINGPATH=$HOME/libdigitalscratch_$VERSION-make_package
DEBPATH=$WORKINGPATH/deb
SOURCEDIR_ORIG=libdigitalscratch_source
ORIGDIR=$(pwd)
DISTRIB=$(lsb_release -cs)
VERSIONPACKAGE=$VERSION-1ppa1~${DISTRIB}1
TARPACK=libdigitalscratch_$VERSION.orig.tar.gz
export DEBEMAIL=julien.rosener@digital-scratch.org
export DEBFULLNAME="Julien Rosener"
export EDITOR=vim

rm -rf $WORKINGPATH
check_error
mkdir -v $WORKINGPATH
check_error
echo ""
echo ""

echo "**************************** Get source code ***************************"
git checkout debian/changelog
check_error
cd ../../
git archive --format zip --output $WORKINGPATH/archive.zip master
unzip $WORKINGPATH/archive.zip -d $WORKINGPATH/$SOURCEDIR_ORIG
rm -v -rf $WORKINGPATH/$SOURCEDIR_ORIG/dist
check_error
echo ""
echo ""

if [[ $2 == --rebuild ]] ; then
    echo "************************* Copy old source package ***********************"
    cp -v $3 $WORKINGPATH
else
    echo "******************** Compress orig source directory *********************"
    ORIGDIR=$(pwd)
    cd $WORKINGPATH
    tar cvzf $TARPACK $SOURCEDIR_ORIG
    cd $ORIGDIR
fi
echo ""
echo ""

echo "**************************** Install debian/ folder ***************************"
cp -v -r dist/ubuntu/debian $WORKINGPATH/$SOURCEDIR_ORIG/
check_error
echo ""
echo ""

echo "************************* Update changelog ******************************"
cd dist/ubuntu/debian
ORIGDIR=$(pwd)
cd $WORKINGPATH/$SOURCEDIR_ORIG
debchange --newversion $VERSIONPACKAGE --distribution $DISTRIB
check_error
cat $WORKINGPATH/$SOURCEDIR_ORIG/debian/changelog
cp $WORKINGPATH/$SOURCEDIR_ORIG/debian/changelog $ORIGDIR
check_error
echo ""
echo ""

echo "***************************** Create Linux base *************************"
export BUILDUSERID=$USER
cd $WORKINGPATH/$SOURCEDIR_ORIG
if [ ! -f ~/pbuilder/$DISTRIB-base.tgz ]
then
    pbuilder-dist $DISTRIB create
fi
pbuilder-dist $DISTRIB update
echo ""
echo ""

echo "************ Parse debian/ config file and create source.changes *********"
debuild -S -sa
check_error
cd ../
echo ""
echo ""

echo "***************************** Create test DEB files *********************"
mkdir -v $DEBPATH
pbuilder-dist $DISTRIB build --buildresult $DEBPATH *.dsc
check_error
echo ""
echo ""

echo "************ Upload source.changes on Launchpad at ppa:$PPAPATH *************"
dput -f ppa:$PPAPATH *source.changes
check_error
echo ""
echo ""

echo "    Done, testing DEBs are in $DEBPATH"
