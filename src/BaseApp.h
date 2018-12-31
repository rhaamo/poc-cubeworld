#ifndef __BaseApp_h_
#define __BaseApp_h_

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreApplicationContext.h"
#include <iostream>
#include <Bites/OgreTrays.h>
#include <OgreWindowEventUtilities.h>
#include <OgreConfigFile.h>
#include <OgreCameraMan.h>

using namespace Ogre;
using namespace OgreBites;

class BaseApp
        : public ApplicationContext,
          public InputListener,
          public OgreBites::TrayListener {
public:
    BaseApp();

    virtual ~BaseApp();

    void setup();

    // OIS::KeyListener
    virtual bool keyPressed( const OgreBites::KeyboardEvent& evt );
    virtual bool keyReleased( const OgreBites::KeyboardEvent& evt );
    // OIS::MouseListener
    virtual bool mouseMoved( const OgreBites::MouseMotionEvent &evt );
    virtual bool mousePressed( const OgreBites::MouseButtonEvent &evt );
    virtual bool mouseReleased( const OgreBites::MouseButtonEvent &evt );

    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const FrameEvent &evt);


protected:
    virtual void createScene(void) = 0; // Override me!

    TrayManager *trayMgr;
    ParamsPanel *detailsPanel;
    Camera *cam;
    CameraMan *camMan;
    Root *root;
    SceneManager *scnMgr;
    bool mShutDown;
};

#endif // #ifndef __BaseApp_h_