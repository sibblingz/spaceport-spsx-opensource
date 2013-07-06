#!/bin/bash -xe

# NOTE: $WORKSPACE is the root of the project git repo

STAGING_AREA="$1"

#force a clean build if indicated
if [ -d $STAGING_AREA/clean.build ]; then
   echo "Performing CLEAN build."
   git clean -fdx
else
   echo "Performing INCREMENTAL build."
fi

#clean old staging area
rm -rf $STAGING_AREA/workspace/spsx
mkdir $STAGING_AREA/workspace/spsx

#clean new staging area
rm -f $STAGING_AREA/workspace/sdk/osx/package/lib/spaceport-support/spsx

#clean old Jenkins artifacts
rm -rf $WORKSPACE/proj/macosx/spsx/build/Debug/spsx-debug.zip
rm -rf $WORKSPACE/proj/macosx/spsx/build/Release/spsx-release.zip

#store component changes in staging area
$SP_SCRIPTS/copy_component_changes.py $WORKSPACE/../builds/$BUILD_NUMBER/changelog.xml $STAGING_AREA/changes/spsx.txt

pushd $WORKSPACE/proj/macosx/spsx
xcodebuild -target spsx -configuration "Debug" build
xcodebuild -target spsx -configuration "Release" build
popd

#archive the build artifacts to the old staging area
mkdir $STAGING_AREA/workspace/spsx/debug
mkdir $STAGING_AREA/workspace/spsx/release
cp $WORKSPACE/proj/macosx/spsx/build/Debug/spsx $STAGING_AREA/workspace/spsx/debug/
cp $WORKSPACE/proj/macosx/spsx/build/Release/spsx $STAGING_AREA/workspace/spsx/release/

#archive the build artifacts to the new staging area
cp $WORKSPACE/proj/macosx/spsx/build/Release/spsx $STAGING_AREA/workspace/sdk/osx/package/lib/spaceport-support/

#recreate Jenkins artifacts
pushd $WORKSPACE/proj/macosx/spsx/build/Debug
zip -r spsx-debug.zip spsx
popd

pushd $WORKSPACE/proj/macosx/spsx/build/Release
zip -r spsx-release.zip spsx
popd

