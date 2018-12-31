#ifndef __BaseApp_h_
#define __BaseApp_h_

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreApplicationContext.h"
#include <iostream>
#include <Bites/OgreTrays.h>

using namespace Ogre;
using namespace OgreBites;

class BaseApp
        : public ApplicationContext
                , public InputListener
{
public:
    BaseApp();
    virtual ~BaseApp();

    void setup();
    bool keyPressed(const KeyboardEvent& evt);
    bool mouseMoved(const MouseMotionEvent& evt);
    bool mousePressed(const MouseButtonEvent& evt);

protected:
    virtual void createScene(void) = 0; // Override me!

    TrayManager *trayMgr;
    ParamsPanel *detailsPanel;
    Camera *cam;
    Root *root;
    SceneManager *scnMgr;
};

#endif // #ifndef __BaseApp_h_