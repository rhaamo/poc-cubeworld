#!/bin/sh
echo '=== OHAI I AM A VERY UGLY SCRIPT'
echo '=== I WILL COPY STUFF FOR YOU'
echo '=== If it fails edit the script to adapt the paths !'

FRAMEWORKS_PATH="/Library/Frameworks"
OGRE_SDK_LIBS_PATH="/Users/rhaamo/src/OgreSDK/lib"

mkdir "CubeWorld.app/Frameworks"
mkdir "CubeWorld.app/Resources"
mkdir "CubeWorld.app/Contents/Plugins"

cp -r "${FRAMEWORKS_PATH}/OGRE.framework" "CubeWorld.app/Frameworks/"
cp -r "${FRAMEWORKS_PATH}/OIS.framework" "CubeWorld.app/Frameworks/"

cp "${OGRE_SDK_LIBS_PATH}/Plugin_BSPSceneManager.dylib" "CubeWorld.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_CgProgramManager.dylib" "CubeWorld.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_OctreeSceneManager.dylib" "CubeWorld.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_OctreeZone.dylib" "CubeWorld.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_PCZSceneManager.dylib" "CubeWorld.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_ParticleFX.dylib" "CubeWorld.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/RenderSystem_GL.dylib" "CubeWorld.app/Contents/Plugins"

cp ../resources/*.cfg .
cp -r ../resources/OGRE .

echo '=== done :-)'
