#include "BaseApp.h"

BaseApp::BaseApp()
    : ApplicationContext("CubeWorldApp")
{
}

BaseApp::~BaseApp(void) {
    if (trayMgr) delete trayMgr;

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


bool BaseApp::keyPressed(const KeyboardEvent& evt)
{

    LogManager::getSingletonPtr()->logMessage("pwessed a key");

    switch (evt.keysym.sym) {
        case SDLK_ESCAPE:
            getRoot()->queueEndRendering();
            break;
        case 'f':
            trayMgr->toggleAdvancedFrameStats();
            break;
        case 'g':
            if (detailsPanel->getTrayLocation() == TL_NONE) {
                trayMgr->moveWidgetToTray(detailsPanel, TL_TOPRIGHT, 0);
                detailsPanel->show();
            } else {
                trayMgr->removeWidgetFromTray(detailsPanel);
                detailsPanel->hide();
            }
            break;
        case 't': {
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
        case 'r': {
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
        case SDLK_F5:
            TextureManager::getSingleton().reloadAll();
            break;
        case 'n':
            //getRenderWindow()->writeContentsToTimestampedFile("screenshot", ".jpg");
            getRenderWindow()->writeContentsToTimestampedFile("screenshot", ".png");
    }

    return true;
}

// lol mouse don't work
bool BaseApp::mouseMoved(const MouseMotionEvent& evt) {
    trayMgr->mouseMoved(evt);
    return true;
}

bool BaseApp::mousePressed(const MouseButtonEvent& evt) {
    trayMgr->mousePressed(evt);
    return true;
}

//! [fullsource]
