#!/bin/bash

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
DATE="$( date '+%Y%m%d%H%S' )"
DIST_DIR=$SCRIPTPATH/dist
LAYOUT_DIR=$SCRIPTPATH/src/wii/res/layout/
BOOT_DOL_SRC=$SCRIPTPATH/boot.*
BOOT_DOL_DEST=$DIST_DIR/APPS/wiimednafen
META_FILE=$DIST_DIR/APPS/wiimednafen/meta.xml

#
# Function that is invoked when the script fails.
#
# $1 - The message to display prior to exiting.
#
function fail() {
    echo $1
    echo "Exiting."
    exit 1
}

# Change to script directory
echo "Changing to script directory..."
cd $SCRIPTPATH || { fail 'Error changing to script directory.'; }

# Build WiiMednafen
echo "Building WiiMednafen..."
make || { fail 'Error building WiiMednafen.'; }

# Create dist directory
if [ ! -d $DIST_DIR ]; then
    echo "Creating dist directory..."
    mkdir -p $DIST_DIR || { fail 'Error creating dist directory.'; }
fi

# Clear dist directory
echo "Clearing dist directory..."
rm -rf $DIST_DIR || { fail 'Error clearing dist directory.'; }

# Copy layout
echo "Copy layout..."
cp -R $LAYOUT_DIR $DIST_DIR || { fail 'Error copying layout.'; }

# Remove .gitignore
echo "Cleaning layout..."
find $DIST_DIR -name .gitignore -type f -delete \
    || { fail 'Error cleaning layout.'; }

# Copy boot files
echo "Copying boot files..."
cp $BOOT_DOL_SRC $BOOT_DOL_DEST || { fail 'Error copying boot.dol.'; }

# Update date in meta file
echo "Setting date in meta file..."
sed -i "s,000000000000,$DATE,g" $META_FILE \
    || { fail 'Error setting date in meta file.'; }

# Update version in meta-file (if SNAPSHOT)
echo "Updating version in meta file..."
sed -i "s,-SNAPSHOT,-SNAPSHOT-$DATE,g" $META_FILE \
    || { fail 'Error setting version in meta file.'; }
    
# Create the distribution (zip)    
echo "Creating distribution..."
VERSION=$( sed -ne "s/.*version>\(.*\)<\/version.*/\1/p" $META_FILE )
VERSION_NO_DOTS="${VERSION//./_}"
DIST_FILE=wiimednafen-$VERSION_NO_DOTS.zip
cd $DIST_DIR || { fail 'Error changing to distribution directory.'; }
zip -r $DIST_FILE . || { fail 'Error creating zip file.'; }
