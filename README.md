##PhysX Project##

#Project Objectives#

The objective of this project was to simply integrate the Nvidia PhysX Physics Engine with my personal C++ game engine. This project features the use of Nvidia PhysX as well as the Nvidia Vehicle SDK. The vehicle has a car controller that currently works only with the use of an xbox controller.
The project also features PhysX joints, articulation and other PhysX features.

NOTE: As this project is integrated in my personal C++ engine, it has been added to this project as a submodule. However, there is a simple setup guide that shows how you can get PhysX running in any other VC++ project provided a rendering engine has been implemented. For a guide on how to setup physX on your project, simply follow the guide below. More posts and information will be posted on my website and updated here for more in depth tutorials.

#Project License#

I will post a formal license soon. Until then please feel free to do what you want with this project's code. 

##PhysX SDK Setup Guide##

In this guide we will discuss how to setup Nvidia PhysX with a Visual C++ project

#Sections to this post

- Section 1. Generating Sample Projects
- Section 2. Linking PhysX Libraries
- Section 3. Setting up Runtimes and Configurations
- Section 4. Using PhysX with your project

##Section 1. Generating Sample Projects

Before we can use Nvidia PhysX with our project, we need to setup the libraries required with our project. To do this you will need to first download and extract the files for Nvidia PhysX. I used version 4.0 but the steps should be the same for other versions of PhysX

#Step 1 : Getting required files and software#

- Download and extract the PhysX SDK from Nvidia's website
- Install CMake
- Install Phython
- Add Python to your system path if it doesn't already exist

#Step 2 : Generating all projects#

- Run the `generate_projects.bat` file to create the visual studio project files for all of PhysX's projects 
- After you have the generated the project files successfully, run the sample projects and make sure everything is working
- Once we have the sample files running, we can move on to the next step which is setting up the libraries required

##Section 2. Linking PhysX Libraries

Now that we have all the source files, lib files and dll files required to use PhysX, we can start linking the libraries required to our projects.
There are different approaches to doing this, one of which is to compile the source code of PhysX with your project. Using source code can be helpful in cases where we would want to view and modify the inner workings of PhysX. For this guide however, we will not be compiling the source code of PhysX, instead we will link the static libraries provided by PhysX to our project in Visual Studio.

#Step 1 : Finding the library files we need#

 PhysX has a ton of features and while these features allow us to explore many interesting usecases, it is recommended to only use the libraries you need and link other libraries as and when they will be required by your project.
 
 This guide will continue to showcase some features of PhysX and hence demonstrate the use of numerous libraries. You may wish to not use some of these libraries or link more libraries depending on your needs.

- Find your libraries in the `physx/bin` folder with your respective build configuration
(I used Windows so my library files were in the `win.x86_64.vc141.mt` folder)

- To use physx, we will need to tell the compiler what files to include when building our projects. 
- The include files are in the `physx/include` folder
- You will need to add this folder to the `additional include directiories` path in your project
	- To do that, you will need to open your visual studio project and right click on your solution, select properties
	- On the properties window select linker in the left tab of configuration properties
	- You will see an `additional include directories` option, add your include path as a new entry in the list

#Step 2 : Telling the linker what we need#

Now that we have added the include directories, we will need to inform the linker to link with these libraries during compile time.
This can be done in 2 ways:
	1. You can open the properties panel for your project and setup `additional dependencies`
		- Properties->Linker->Input->Additional Dependencies

 	2. You can link to the libraries using code.
 		- For this you will need to add the following to your code

```cpp
//PhysX Pragma Comments
#if ( defined( _WIN64 ) & defined( _DEBUG ) )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysX_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXCommon_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXCooking_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXFoundation_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXPvdSDK_static_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXVehicle_static_64.lib" )
#elif ( defined ( _WIN64 ) & defined( NDEBUG ) )
#pragma comment( lib, "PhysX/lib/release_x64/PhysX_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXCommon_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXCooking_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXFoundation_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXPvdSDK_static_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXVehicle_static_64.lib" )
#endif
```

##NOTE##
In the code snippet above, the #pragma comment(lib, < file_path >) tells us what libraries we wish to link for the x64 platform for the Debug and Release build configurations. You will need to set this up for the different configurations we intend to use with our project. The next section will go into more detail on configurations and how you may set them up

##Section 3. Setting up Runtimes and Configurations

In this section we will talk about the runtimes we will use with PhysX and the configuration options we may use with our project.

#Step 1 : Runtimes for Build configurations

Your project will have 1 or more build configurations depending on your project setup. For my project, I have setup a Debug and a Release build configurations for both the windows x86 and x64 platforms. Your configurations may vary depending on your usecase but this guide assumes you have atleast one build configuration setup.

To setup a new build configuration, you may perform the steps below:
- Right click on your project and select properties
- Click on the Configuration Manager button on the top right corner
- In your Configuration Manager window, you may use the drop downs on `solution_configuration` and `solution_platform` to create a new configuration of your choice

##NOTE##
PhysX supports 4 different build configurations. They are Debug, Release, Checked and Profile. Information on the various build configurations supported can be found on the PhysX API documentation online.

#Step 2 : Code generation settings

Once you have your build configurations setup, you will need to setup your `runtime_library` settings for the project.

To do so, follow the steps below:
- Open your project properties tab
- Select the C/C++ drop down in the configuration properties tab on the left
- Select Code Generation option

##NOTE##
Now there are a few things to consider when you are in this window. You will need to setup different runtime options for your different build configurations. The Release code for PhysX uses `Multi Threaded (/MT)` setting while the Debug configuration uses `Multi Threaded Debug (/MTd)` option.

- Select your build configuration from the drop down on the top left
- Setup the appropriate `Runtime Library` option for your configuration (/MT or /MTd)

With that you have your runtime libraries setup for PhysX. You may compile your code to make sure your existing code base is working as exepected.

#Step 3 : Linking required libraries appropriately

Now although we performed the linking of libraries in the previous section, we will double check that we are still linking liraries for our new build configurations for all our supported platforms.

To do so, please refer again to Section 2, step 2 to make sure you have your libraries setup for your different configurations. The code I used is as follows:

```cpp
//PhysX Pragma Comments
#if ( defined( _WIN64 ) & defined( _DEBUG ) )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysX_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXCommon_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXCooking_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXFoundation_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXPvdSDK_static_64.lib" )
#pragma comment( lib, "PhysX/lib/debug_x64/PhysXVehicle_static_64.lib" )
#elif ( defined ( _WIN64 ) & defined( NDEBUG ) )
#pragma comment( lib, "PhysX/lib/release_x64/PhysX_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXCommon_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXCooking_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXFoundation_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXPvdSDK_static_64.lib" )
#pragma comment( lib, "PhysX/lib/release_x64/PhysXVehicle_static_64.lib" )
#elif ( defined( _WIN32 ) & defined( _DEBUG ) )
#pragma comment( lib, "PhysX/lib/debug_x86/PhysX_32.lib" )
#pragma comment( lib, "PhysX/lib/debug_x86/PhysXCommon_32.lib" )
#pragma comment( lib, "PhysX/lib/debug_x86/PhysXCooking_32.lib" )
#pragma comment( lib, "PhysX/lib/debug_x86/PhysXExtensions_static_32.lib" )
#pragma comment( lib, "PhysX/lib/debug_x86/PhysXFoundation_32.lib" )
#pragma comment( lib, "PhysX/lib/debug_x86/PhysXPvdSDK_static_32.lib" )
#pragma comment( lib, "PhysX/lib/debug_x86/PhysXVehicle_static_32.lib" )
#elif ( defined( _WIN32 ) & defined( NDEBUG ) )
#pragma comment( lib, "PhysX/lib/release_x86/PhysX_32.lib" )
#pragma comment( lib, "PhysX/lib/release_x86/PhysXCommon_32.lib" )
#pragma comment( lib, "PhysX/lib/release_x86/PhysXCooking_32.lib" )
#pragma comment( lib, "PhysX/lib/release_x86/PhysXExtensions_static_32.lib" )
#pragma comment( lib, "PhysX/lib/release_x86/PhysXFoundation_32.lib" )
#pragma comment( lib, "PhysX/lib/release_x86/PhysXPvdSDK_static_32.lib" )
#pragma comment( lib, "PhysX/lib/release_x86/PhysXVehicle_static_32.lib" )
#endif
```

The above snippet shows the code required to link the libraries for both the Release and Debug configurations for the x86 or x64 platforms respectively. Your setup may vary.

With this you should have the required Nvidia PhysX static libraries setup with your project.

##Section 4. Using PhysX with your project

This is where things get fun. Setting up PhysX is simple and easy, all we need to do is make sure we start up PhysX when our program starts, run some logic to update PhysX every frame and finally have a shut down step that will close PhysX.

#Step 1 : The Start up logic

First we will include the required headers for PhysX. You may include each module seperately but for this guide, I will simply include the `PxPhysicsAPI.h` header to include all the PhysX files we need in our code base

```cpp
//PhysX API
#include "ThirdParty/PhysX/include/PxPhysicsAPI.h"
```

Next we will run an initialization function that will initialize PhysX at the start of our project. This is what my startup function looks like for reference.

```cpp
//------------------------------------------------------------------------------------------------------------------------------
// Variables we will need
	PxDefaultAllocator					m_PxAllocator;
	PxDefaultErrorCallback				m_PXErrorCallback;

	PxFoundation*						m_PxFoundation = nullptr;
	PxCooking*							m_PxCooking = nullptr;
	PxPhysics*							m_PhysX = nullptr;

	PxDefaultCpuDispatcher*				m_PxDispatcher = nullptr;
	PxScene*							m_PxScene = nullptr;

	PxMaterial*							m_PxDefaultMaterial = nullptr;

	//PhysX Visual Debugger
	PxPvd*								m_Pvd = nullptr;
	std::string							m_pvdIPAddress = "127.0.0.1";
	int									m_pvdPortNumber = 5425;
	uint								m_pvdTimeOutSeconds = 10;

	//Default values for PhysX properties to use
	float								m_defaultStaticFriction = 0.5f;
	float								m_defaultDynamicFriction = 0.5f;
	float								m_defaultRestitution = 0.6f;
	float								m_defaultDensity = 10.f;
	float								m_defaultAngularDamping = 0.5f;

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::StartUp()
{
	//PhysX starts off by setting up a Physics Foundation
	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);

	//Setup PhysX cooking if you need convex hull cooking support or other cooking features
	m_PxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_PxFoundation, PxCookingParams(PxTolerancesScale()));

	//Create the PhysX Visual Debugger by giving it the current foundation
	m_Pvd = PxCreatePvd(*m_PxFoundation);
	//The PVD needs connection via a socket. It will run on the Address defined, in our case it's our machine
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(m_pvdIPAddress.c_str(), m_pvdPortNumber, m_pvdTimeOutSeconds);
	m_Pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	//Create Physics! This creates an instance of the PhysX SDK
	m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);
	PxInitExtensions(*m_PhysX, m_Pvd);

	//What is the description of this PhysX scene?
	PxSceneDesc sceneDesc(m_PhysX->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	//This creates CPU dispatcher threads or worker threads. We will make 2
	m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_PxDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	//Create the scene now by passing the scene's description
	m_PxScene = m_PhysX->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
	if (pvdClient)
	{
		//I have a PVD client, so set some flags that it needs
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	m_PxDefaultMaterial = m_PhysX->createMaterial(m_defaultStaticFriction, m_defaultDynamicFriction, m_defaultRestitution);
}
```

##NOTE##
You will find the PhysX samples we compiled helpful here. You can find the startup logic for various different projects and how PhysX is initialized for differnet features.

#Step 2 : Frame Update Logic

During each frame of our program we will need to call some functions to tell physx to update. This logic will allow PhysX to update the various parameters for it's rigidbodies and other physics objects.

To do so, we simply call 2 lines of code in our update function like so:

```cpp
//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::Update(float deltaTime)
{
	m_PxScene->simulate(deltaTime);
	m_PxScene->fetchResults(true);
}
```

The `simulate` function runs a simulation for the time we pass in as a parameter. In this case we will pass the `deltaTime` which is the time taken by each frame of our program.

The `fetchResults` function will fetch simulation results from PhysX for all objects in our scene.

#Step 3 : Shutting down PhysX

Finally when we are closing our program, we want to shut down PhysX appropriately

To do so, we will need to release the memory taken up by some PhysX system objects. To simplify our code, we can use a macro that performs the release of memeory for any object passed to it.

The macro is as follows (can be found in the sample code provided by PhysX)

```cpp
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}
```

Now we will simply create a ShutDown function that is called when our program is closing. The function looks as follows:

```cpp

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::ShutDown()
{
	PX_RELEASE(m_PxScene);
	PX_RELEASE(m_PxDispatcher);
	PX_RELEASE(m_PhysX);
	PX_RELEASE(m_PxCooking);

	if (m_Pvd)
	{
		PxPvdTransport* transport = m_Pvd->getTransport();
		m_Pvd->release();	m_Pvd = NULL;
		PX_RELEASE(transport);
	}

	//Release the PhysX foundation last as other things depend on it
	PX_RELEASE(m_PxFoundation);
}
```

##NOTE##
We want to release the PhysX Foundation last as other PhysX objects depend on it.

With this logic added to our program, we are successfully initializing, using and shutting down our physics simulation. Compile your program to make sure everything is working as expected. In the next post, we will discuss how to setup a simple PhysX simulation using rigidbodies in the world.# ProtoPhysX

This is a PhysX integration project using the D3D11 Engine
