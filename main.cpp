// main.cpp: Based on the original tutorial 02.
//           See LICENSE file for licensing details

/** Example 002 Quake3Map

This tutorial shows how to load a Quake 3 map into the engine, create a
SceneNode for optimizing the speed of rendering, and how to create a user
controlled camera.

Please note that you should know the basics of the engine before starting this
tutorial. Just take a short look at the first tutorial, if you haven't done
this yet: http://irrlicht.sourceforge.net/docu/example001.html

Lets start like the HelloWorld example: We include the irrlicht header files
and an additional file to be able to ask the user for a driver type using the
console.
*/
#include <irrlicht.h>

/*
As already written in the HelloWorld example, in the Irrlicht Engine everything
can be found in the namespace 'irr'. To get rid of the irr:: in front of the
name of every class, we tell the compiler that we use that namespace from now
on, and we will not have to write that 'irr::'. There are 5 other sub
namespaces 'core', 'scene', 'video', 'io' and 'gui'. Unlike in the HelloWorld
example, we do not call 'using namespace' for these 5 other namespaces, because
in this way you will see what can be found in which namespace. But if you like,
you can also include the namespaces like in the previous example.
*/
using namespace irr;

using namespace gui;
using namespace core;

// Joystick event receiver
class MyEventReceiver : public IEventReceiver
{
public:
	SEvent::SJoystickEvent current;
	bool isValid = false;
	virtual bool OnEvent(const SEvent &ev)
	{
		if (ev.EventType == irr::EET_JOYSTICK_INPUT_EVENT)
		{
			current = ev.JoystickEvent;
			isValid = true;
		}
		return false;
	}
};

extern "C" int SDL_main(int ac, char** av)
{
	// create device and exit if creation failed

	MyEventReceiver theEventReceiver;

	//	video::E_DRIVER_TYPE myvideo = video::EDT_BURNINGSVIDEO;
	video::E_DRIVER_TYPE myvideo = video::EDT_OGLES2;


	IrrlichtDevice *device =
		createDevice(myvideo, core::dimension2d<u32>(640, 480),
		32, false, false, false, &theEventReceiver);

	if (device == 0)
		return 1; // could not create selected driver.

	core::array<SJoystickInfo> ji;
	device->activateJoysticks(ji);

	/*
	Get a pointer to the video driver and the SceneManager so that
	we do not always have to call irr::IrrlichtDevice::getVideoDriver() and
	irr::IrrlichtDevice::getSceneManager().
	*/
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	/*
	To display the Quake 3 map, we first need to load it. Quake 3 maps
	are packed into .pk3 files which are nothing else than .zip files.
	So we add the .pk3 file to our irr::io::IFileSystem. After it was added,
	we can read from the files in that archive as if they were stored on disk.
	*/
	device->getFileSystem()->addFileArchive("media/map-20kdm2.pk3");

	/*
	Now we can load the mesh by calling	irr::scene::ISceneManager::getMesh(). 
	We get a pointer returned to an	irr::scene::IAnimatedMesh. Quake 3 maps are 
	not	really animated, they are only a chunk of static geometry with
	some materials attached. Hence the IAnimatedMesh consists of only one
	frame, so we get the "first frame" of the "animation", which is our
	quake level and create an Octree scene node with it, using
	irr::scene::ISceneManager::addOctreeSceneNode().
	The Octree optimizes the scene a little bit, trying to draw only geometry
	which is currently visible. An alternative to the Octree would be a
	irr::scene::IMeshSceneNode, which would always draw the complete
	geometry of the mesh, without optimization. Try it: Use
	irr::scene::ISceneManager::addMeshSceneNode() instead of
	addOctreeSceneNode() and compare the primitives drawn by the video
	driver. (There is a irr::video::IVideoDriver::getPrimitiveCountDrawn()
	method in the irr::video::IVideoDriver class). Note that this
	optimization with the Octree is only useful when drawing huge meshes
	consisting of lots of geometry and if users can't see the whole scene at 
	once.
	*/
	scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	scene::ISceneNode* node = 0;

	if (mesh)
		node = smgr->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 1024);
//		node = smgr->addMeshSceneNode(mesh->getMesh(0));

	/*
	Because the level was not modeled around the origin (0,0,0), we
	translate the whole level a little bit. This is done on
	irr::scene::ISceneNode level using the methods
	irr::scene::ISceneNode::setPosition() (in this case),
	irr::scene::ISceneNode::setRotation(), and
	irr::scene::ISceneNode::setScale().
	*/
	if (node)
		node->setPosition(core::vector3df(-1300,-144,-1249));

	/*
	Now we need a camera to look at the Quake 3 map.
	We want to create a user controlled camera. There are some
	cameras available in the Irrlicht engine. For example the
	MayaCamera which can be controlled like the camera in Maya:
	Rotate with left mouse button pressed, Zoom with both buttons pressed,
	translate with right mouse button pressed. This could be created with
	irr::scene::ISceneManager::addCameraSceneNodeMaya(). But for this
	example, we want to create a camera which behaves like the ones in
	first person shooter games (FPS) and hence use
	irr::scene::ISceneManager::addCameraSceneNodeFPS().
	*/
	scene::ICameraSceneNode* cam;
	//cam = smgr->addCameraSceneNodeFPS();
	cam = smgr->addCameraSceneNode();
	cam->bindTargetAndRotation(true);

	/*
	The mouse cursor needs not be visible, so we hide it via the
	irr::IrrlichtDevice::ICursorControl.
	*/
	//device->getCursorControl()->setVisible(false);

	/*
	Everything is set up, so lets draw it. We also write the current
	frames per second and the primitives drawn into the caption of the
	window. The test for irr::IrrlichtDevice::isWindowActive() is optional,
	but prevents the engine to grab the mouse cursor after task switching
	when other programs are active. The call to	irr::IrrlichtDevice::yield()
	will avoid the busy loop to eat up all CPU cycles when the window is not 
	active.
	*/
	int lastFPS = -1;
	IGUIEnvironment* guienv = device->getGUIEnvironment();
	IGUIStaticText* label = guienv->addStaticText(L"Hello World!",
		rect<s32>(10, 10, 450, 22), true);
	label->setDrawBackground(true);
	label->enableOverrideColor(true);
	label->setOverrideColor(video::SColor(255, 255, 255, 255));
	label->setBackgroundColor(video::SColor(255, 0, 0, 0));
	
	int frames = 0;

	while(device->run())
	{
		if (device->isWindowActive())
		{
			frames++;
			f32 h = -1;
			f32 v = -1;

			if (theEventReceiver.isValid)
			{
				h = (f32)theEventReceiver.current.Axis[SEvent::SJoystickEvent::AXIS_Y] / 32767.0f;
				v = (f32)theEventReceiver.current.Axis[SEvent::SJoystickEvent::AXIS_X] / 32767.0f;

				// Rotate camera
				core::quaternion rot(h * 1.0f * core::PI, v * 1.0f * core::PI, 0);
				core::vector3df newrot;
				rot.toEuler(newrot);
				cam->setRotation(newrot*core::RADTODEG);
			}

			driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(255,200,200,200));
			smgr->drawAll();
			guienv->drawAll();
			driver->endScene();
			int fps = driver->getFPS();

			core::stringw str = L"Irrlicht Engine - Quake 3 Map example [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;

			str += " frame: ";
			str += frames;

			str += " h: ";
			str += h;
			str += " v: ";
			str += v;

			label->setText(str.c_str());
			lastFPS = fps;
		}
		else
			device->yield();
	}

	/*
	In the end, delete the Irrlicht device.
	*/
	device->drop();
	return 0;
}

/*
That's it. Compile and play around with the program.
**/
