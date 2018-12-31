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

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>


using namespace Ogre;
using namespace OgreBites;

class BaseApp
        : public ApplicationContext,
          public InputListener,
          public OIS::KeyListener,
          public OIS::MouseListener,
          public OgreBites::TrayListener {
public:
    BaseApp();

    virtual ~BaseApp();

    void setup();

    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    //Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed(RenderWindow *rw);

    //Adjust mouse clipping area
    virtual void windowResized(RenderWindow *rw);

    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const FrameEvent &evt);

    virtual void createFrameListener(void);


protected:
    virtual void createScene(void) = 0; // Override me!

    TrayManager *trayMgr;
    ParamsPanel *detailsPanel;
    Camera *cam;
    CameraMan *camMan;
    Root *root;
    SceneManager *scnMgr;
    bool mShutDown;

    //OIS Input devices
    OIS::InputManager *mInputManager;
    OIS::Mouse *mMouse;
    OIS::Keyboard *mKeyboard;

};

#endif // #ifndef __BaseApp_h_