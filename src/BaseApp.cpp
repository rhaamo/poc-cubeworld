#include <Bites/OgreWindowEventUtilities.h>
#include "BaseApp.h"

BaseApp::BaseApp()
    : ApplicationContext("CubeWorldApp"),
      mShutDown(false)
{
}

BaseApp::~BaseApp(void) {
    if (trayMgr) delete trayMgr;
    if (camMan) delete camMan;

    //Remove ourself as a Window listener
    windowClosed(getRenderWindow());
    delete root;
}

void BaseApp::setup()
{
    // do not forget to call the base first
    ApplicationContext::setup();
    addInputListener(this);

    // - Create a Scene
    // get a pointer to the already created root
    root = getRoot();
    scnMgr = root->createSceneManager();

    ConfigFile cf;
    cf.load("resources.cfg");
    ConfigFile::SectionIterator secIt = cf.getSectionIterator();
    while (secIt.hasMoreElements()) {
        ConfigFile::SettingsMultiMap* settings = secIt.getNext();
        ConfigFile::SettingsMultiMap::iterator it;

        for (it = settings->begin(); it != settings->end(); ++it) {
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(it->second, it->first);
        }
    }

    // register our scene with the RTSS
    RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    // - Create a Camera
    //! [camera]
    SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    cam = scnMgr->createCamera("myCam");

    // Position it at 500 in Z direction
    camNode->setPosition(Vector3(-31,140,202));
    // Look back along -Z
    camNode->lookAt(Vector3(0.084, -0.6, 0.05), Node::TransformSpace::TS_WORLD);
    cam->setNearClipDistance(5);
    camNode->attachObject(cam);

    // Create a camera manager
    camMan = new CameraMan(camNode);

    // - Create viewports
    // Create one viewport, entire window
    Viewport *vp = getRenderWindow()->addViewport(cam);

    vp->setBackgroundColour(ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    cam->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

    // - Set default mipmap level (NB some APIs ignore this)
    TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // - CubeWorld create scene
    createScene(); // reside in CubeWorld.cpp, full scene init

    // - OIS thingy
    createFrameListener();

    // - Tray manager thing

    trayMgr = new TrayManager("InterfaceName", getRenderWindow());
    trayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    trayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    trayMgr->toggleAdvancedFrameStats();
    trayMgr->showCursor();

    // Create a panel to display sample details
    StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
    items.push_back("");
    items.push_back("Filtering");
    items.push_back("Poly Mode");

    detailsPanel = trayMgr->createParamsPanel(TL_NONE, "DetailsPanel", 200, items);
    detailsPanel->setParamValue(9, "Bilinear");
    detailsPanel->setParamValue(10, "Solid");
    detailsPanel->show();

    addInputListener(trayMgr); // relay inputs to the tray manager

    //getRoot()->addFrameListener(this);
}

void BaseApp::createFrameListener(void) {
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    getRenderWindow()->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(getRenderWindow());

    //Register as a Window listener
    //WindowEventUtilities::addWindowEventListener(getRenderWindow(), this);

    mRoot->addFrameListener(this);
}

bool BaseApp::frameRenderingQueued(const FrameEvent& evt) {
    if(getRenderWindow()->isClosed())
        return false;

    if(mShutDown)
        return false;

    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    // TODO FIXME this thing
    //mTrayMgr->frameRenderingQueued(evt);
    trayMgr->frameRendered(evt);

    if (!trayMgr->isDialogVisible())
    {
        // this one too FIXME TODO
        //mCameraMan->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera
        camMan->frameRendered(evt);
        if (detailsPanel->isVisible())   // if details panel is visible, then update its contents
        {
            detailsPanel->setParamValue(0, Ogre::StringConverter::toString(cam->getDerivedPosition().x));
            detailsPanel->setParamValue(1, Ogre::StringConverter::toString(cam->getDerivedPosition().y));
            detailsPanel->setParamValue(2, Ogre::StringConverter::toString(cam->getDerivedPosition().z));
            detailsPanel->setParamValue(4, Ogre::StringConverter::toString(cam->getDerivedOrientation().w));
            detailsPanel->setParamValue(5, Ogre::StringConverter::toString(cam->getDerivedOrientation().x));
            detailsPanel->setParamValue(6, Ogre::StringConverter::toString(cam->getDerivedOrientation().y));
            detailsPanel->setParamValue(7, Ogre::StringConverter::toString(cam->getDerivedOrientation().z));
        }
    }

    return true;
}

bool BaseApp::keyPressed(const OIS::KeyEvent &arg)
{

    LogManager::getSingletonPtr()->logMessage("pwessed a key");

    switch (arg.key) {
        case OIS::KC_ESCAPE:
            //getRoot()->queueEndRendering();
            mShutDown = true;
            break;
        case OIS::KC_F:
            trayMgr->toggleAdvancedFrameStats();
            break;
        case OIS::KC_G:
            if (detailsPanel->getTrayLocation() == TL_NONE) {
                trayMgr->moveWidgetToTray(detailsPanel, TL_TOPRIGHT, 0);
                detailsPanel->show();
            } else {
                trayMgr->removeWidgetFromTray(detailsPanel);
                detailsPanel->hide();
            }
            break;
        case OIS::KC_T: {
            String newVal;
            TextureFilterOptions tfo;
            unsigned int aniso;

            switch (detailsPanel->getParamValue(9).asUTF8()[0]) {
                case 'B':
                    newVal = "Trilinear";
                    tfo = Ogre::TFO_TRILINEAR;
                    aniso = 1;
                    break;
                case 'T':
                    newVal = "Anisotropic";
                    tfo = Ogre::TFO_ANISOTROPIC;
                    aniso = 8;
                    break;
                case 'A':
                    newVal = "None";
                    tfo = Ogre::TFO_NONE;
                    aniso = 1;
                    break;
                default:
                    newVal = "Bilinear";
                    tfo = Ogre::TFO_BILINEAR;
                    aniso = 1;
            }

            MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
            MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
            detailsPanel->setParamValue(9, newVal);
            break;
        }
        case OIS::KC_R: {
            String newValPol;
            PolygonMode pm;
            switch (cam->getPolygonMode()) {
                case Ogre::PM_SOLID:
                    newValPol = "Wireframe";
                    pm = Ogre::PM_WIREFRAME;
                    break;
                case Ogre::PM_WIREFRAME:
                    newValPol = "Points";
                    pm = Ogre::PM_POINTS;
                    break;
                default:
                    newValPol = "Solid";
                    pm = Ogre::PM_SOLID;
            }

            cam->setPolygonMode(pm);
            detailsPanel->setParamValue(10, newValPol);
            break;
        }
        case OIS::KC_F5:
            TextureManager::getSingleton().reloadAll();
            break;
        case OIS::KC_N:
            //getRenderWindow()->writeContentsToTimestampedFile("screenshot", ".jpg");
            getRenderWindow()->writeContentsToTimestampedFile("screenshot", ".png");
    }

    //camMan->keyPressed(arg);
    return true;
}

bool BaseApp::keyReleased( const OIS::KeyEvent &arg )
{
    //camMan->keyReleased(arg);
    return true;
}

bool BaseApp::mouseMoved( const OIS::MouseEvent &arg )
{
    //if (trayMgr->mouseMoved(arg)) return true;
    //camMan->mouseMoved(arg);
    return true;
}

bool BaseApp::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    //if (trayMgr->mousePressed(arg)) return true;
    //camMan->mousePressed(arg);
    return true;
}

bool BaseApp::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    ///if (trayMgr->mouseReleased(arg)) return true;
    //camMan->mouseReleased(arg);
    return true;
}

//Adjust mouse clipping area
void BaseApp::windowResized(RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}


//Unattach OIS before window shutdown (very important under Linux)
void BaseApp::windowClosed(RenderWindow* rw) {
    //Only close for window that created OIS (the main window in these demos)
    if( rw == getRenderWindow() )     {
        if( mInputManager )         {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

