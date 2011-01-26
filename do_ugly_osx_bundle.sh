#!/bin/sh
echo '=== OHAI I AM A VERY UGLY SCRIPT'
echo '=== I WILL COPY STUFF FOR YOU'
echo '=== If it fails edit the script to adapt the paths !'

FRAMEWORKS_PATH="/Library/Frameworks"
OGRE_SDK_LIBS_PATH="/Users/rhaamo/src/OgreSDK/lib"

mkdir "OGRETutorialFramework.app/Frameworks"
mkdir "OGRETutorialFramework.app/Resources"
mkdir "OGRETutorialFramework.app/Contents/Plugins"

cp -r "${FRAMEWORKS_PATH}/OGRE.framework" "OGRETutorialFramework.app/Frameworks/"
cp -r "${FRAMEWORKS_PATH}/OIS.framework" "OGRETutorialFramework.app/Frameworks/"

cp "${OGRE_SDK_LIBS_PATH}/Plugin_BSPSceneManager.dylib" "OGRETutorialFramework.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_CgProgramManager.dylib" "OGRETutorialFramework.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_OctreeSceneManager.dylib" "OGRETutorialFramework.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_OctreeZone.dylib" "OGRETutorialFramework.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_PCZSceneManager.dylib" "OGRETutorialFramework.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/Plugin_ParticleFX.dylib" "OGRETutorialFramework.app/Contents/Plugins"
cp "${OGRE_SDK_LIBS_PATH}/RenderSystem_GL.dylib" "OGRETutorialFramework.app/Contents/Plugins"

cp ../resources/*.cfg .
cp -r ../resources/OGRE .

echo '=== done :-)'
