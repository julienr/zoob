#!/bin/bash
DEMO_DIR="demo"

#directories
rm -rf $DEMO_DIR
mkdir $DEMO_DIR

#copy what is needed
for dir in "assets" "libs" "res" "src" "gen"
do
  cp -r project/$dir $DEMO_DIR/
done

#create demo java stubs
mkdir -p $DEMO_DIR/src/net/fhtagn/zoob_demo/
echo " 
package net.fhtagn.zoob_demo; 
public class Zoob extends net.fhtagn.zoobgame.Zoob {}" > $DEMO_DIR/src/net/fhtagn/zoob_demo/Zoob.java

echo " 
package net.fhtagn.zoob_demo; 
public class ZoobApplication extends net.fhtagn.zoobgame.ZoobApplication {
  @Override
  protected boolean isDemo () {
    return true;
  }
}" > $DEMO_DIR/src/net/fhtagn/zoob_demo/ZoobApplication.java

#copy and modify android manifest
cat project/AndroidManifest.xml | sed -e "s/package=\"net.fhtagn.zoobgame\"/package=\"net.fhtagn.zoob_demo\"/" > $DEMO_DIR/AndroidManifest.xml

#need build.properties to sign
cp project/build.properties $DEMO_DIR/

#create build.xml and compile
pushd $DEMO_DIR
android update project --name zoob_demo --target android-7 --path .
ant release
popd
