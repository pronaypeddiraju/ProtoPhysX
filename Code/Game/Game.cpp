//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Game.hpp"
//Engine Systems
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vertex_Lit.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/TextureView.hpp"
//PhysX Includes
#include "ThirdParty/PhysX/include/PxPhysicsAPI.h"

//PhysX Pragma Comments
#if ( defined( _WIN64 ) & defined( _DEBUG ) )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysX_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXCommon_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXCooking_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXFoundation_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXPvdSDK_static_64.lib" )
#elif ( defined ( _WIN64 ) & defined( NDEBUG ) )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysX_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXCommon_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXCooking_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXFoundation_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXPvdSDK_static_64.lib" )
#elif ( defined( _WIN32 ) & defined( _DEBUG ) )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysX_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXCommon_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXCooking_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXExtensions_static_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXFoundation_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXPvdSDK_static_32.lib" )
#elif ( defined( _WIN32 ) & defined( NDEBUG ) )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysX_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXCommon_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXCooking_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXExtensions_static_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXFoundation_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXPvdSDK_static_32.lib" )
#endif

//------------------------------------------------------------------------------------------------------------------------------
float g_shakeAmount = 0.0f;

RandomNumberGenerator* g_randomNumGen;
extern RenderContext* g_renderContext;
extern AudioSystem* g_audio;
bool g_debugMode = false;

//------------------------------------------------------------------------------------------------------------------------------
Game::Game()
{
	m_isGameAlive = true;
	m_testAudioID = g_audio->CreateOrGetSound("Data/Audio/UproarLilWayne.mp3");

	m_squirrelFont = g_renderContext->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");

	g_devConsole->SetBitmapFont(*m_squirrelFont);
	g_debugRenderer->SetDebugFont(m_squirrelFont);
}

//------------------------------------------------------------------------------------------------------------------------------
Game::~Game()
{
	m_isGameAlive = false;
	Shutdown();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::StartUp()
{
	SetupMouseData();
	SetupCameras();

	GetandSetShaders();
	LoadGameTextures();

	LoadGameMaterials();

	g_devConsole->PrintString(Rgba::BLUE, "this is a test string");
	g_devConsole->PrintString(Rgba::RED, "this is also a test string");
	g_devConsole->PrintString(Rgba::GREEN, "damn this dev console lit!");
	g_devConsole->PrintString(Rgba::WHITE, "Last thing I printed");

	g_eventSystem->SubscribeEventCallBackFn("TestEvent", TestEvent);

	g_eventSystem->SubscribeEventCallBackFn("ToggleLight1", ToggleLight1);
	g_eventSystem->SubscribeEventCallBackFn("ToggleLight2", ToggleLight2);
	g_eventSystem->SubscribeEventCallBackFn("ToggleLight3", ToggleLight3);
	g_eventSystem->SubscribeEventCallBackFn("ToggleLight4", ToggleLight4);
	g_eventSystem->SubscribeEventCallBackFn("ToggleAllPointLights", ToggleAllPointLights);

	CreateInitialMeshes();

	CreateInitialLight();

	//Only to keep track of what input does what
	DebugRenderOptionsT options;
	options.space = DEBUG_RENDER_SCREEN;

	SetupPhysX(true);

}

//------------------------------------------------------------------------------------------------------------------------------
void Game::SetupMouseData()
{
	//IntVec2 clientCenter = g_windowContext->GetClientCenter();
	//g_windowContext->SetClientMousePosition(clientCenter);

	g_windowContext->SetMouseMode(MOUSE_MODE_ABSOLUTE);
	//g_windowContext->HideMouse();
}

void Game::SetupCameras()
{
	//Create the Camera and setOrthoView
	m_mainCamera = new Camera();
	m_mainCamera->SetColorTarget(nullptr);

	//Create a devConsole Cam
	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetColorTarget(nullptr);

	//Set Projection Perspective for new Cam
	m_camPosition = Vec3(10.f, 50.f, -100.f);
	m_mainCamera->SetColorTarget(nullptr);
	m_mainCamera->SetPerspectiveProjection( m_camFOVDegrees, 0.1f, 100.0f, SCREEN_ASPECT);

	m_clearScreenColor = new Rgba(0.f, 0.f, 0.5f, 1.f);
}

void Game::SetStartupDebugRenderObjects()
{
	ColorTargetView* ctv = g_renderContext->GetFrameColorTarget();
	//Setup debug render client data
	g_debugRenderer->SetClientDimensions( ctv->m_height, ctv->m_width );

	//Setup Debug Options
	DebugRenderOptionsT options;
	options.mode = DEBUG_RENDER_ALWAYS;
	options.beginColor = Rgba::BLUE;
	options.endColor = Rgba::RED;

	//------------------------------------------------------------------------------------------------------------------------------
	// 2D Objects
	//------------------------------------------------------------------------------------------------------------------------------

	//Make 2D Point on screen
	g_debugRenderer->DebugRenderPoint2D(options, Vec2(10.f, 10.f), 5.0f);
	//Make 2D Point at screen center
	options.beginColor = Rgba::BLUE;
	options.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderPoint2D(options, Vec2(0.f, 0.f), 10.f);

	options.beginColor = Rgba::YELLOW;
	options.endColor = Rgba::RED;
	//Draw a line in 2D screen space
	g_debugRenderer->DebugRenderLine2D(options, Vec2(ctv->m_width * -0.5f, ctv->m_height * -0.5f), Vec2(-150.f, -150.f), 20.f);

	//Draw a quad in 2D screen space
	options.beginColor = Rgba::GREEN;
	options.endColor = Rgba::RED;
	g_debugRenderer->DebugRenderQuad2D(options, AABB2(Vec2(-150.f, -150.f), Vec2(-100.f, -100.f)), 20.f);

	//Textured Quad
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::RED;
	g_debugRenderer->DebugRenderQuad2D(options, AABB2(Vec2(-200.f, -200.f), Vec2(-150.f, -150.f)), 20.f, m_textureTest);

	//Disc2D
	options.beginColor = Rgba::DARK_GREY;
	options.endColor = Rgba::ORANGE;
	g_debugRenderer->DebugRenderDisc2D(options, Disc2D(Vec2(100.f, 100.f), 25.f), 10.f);

	//Ring2D
	options.beginColor = Rgba::ORANGE;
	options.endColor = Rgba::DARK_GREY;
	g_debugRenderer->DebugRenderRing2D(options, Disc2D(Vec2(100.f, 100.f), 25.f), 10.f, 5.f);

	//Ring2D
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderRing2D(options, Disc2D(Vec2(150.f, 100.f), 2000.f), 10.f, 1.f);

	//Wired Quad
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderWireQuad2D(options, AABB2(Vec2(100.f, -100.f), Vec2(150.f, -50.f)), 20.f);

	//Text
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::RED;
	const char* text2D = "Read me bruh";
	g_debugRenderer->DebugRenderText2D(options, Vec2(-100.f, 200.f), Vec2(100.f, 200.f), text2D, DEFAULT_TEXT_HEIGHT, 20.f);

	//Arrow 2D
	options.beginColor = Rgba::GREEN;
	options.endColor = Rgba::GREEN;
	g_debugRenderer->DebugRenderArrow2D(options, Vec2(0.f, 0.f), Vec2(200.f, 200.f), 20.f, 5.f);

	//Arrow 2D
	options.beginColor = Rgba::BLUE;
	options.endColor = Rgba::BLUE;
	g_debugRenderer->DebugRenderArrow2D(options, Vec2(0.f, 0.f), Vec2(200.f, -200.f), 20.f, 5.f);

	//------------------------------------------------------------------------------------------------------------------------------
	// 3D Objects
	//------------------------------------------------------------------------------------------------------------------------------
	
	DebugRenderOptionsT options3D;
	options3D.space = DEBUG_RENDER_WORLD;
	options3D.beginColor = Rgba::GREEN;
	options3D.endColor = Rgba::RED;

	options3D.mode = DEBUG_RENDER_XRAY;
	//make a 3D point
	g_debugRenderer->DebugRenderPoint(options3D, Vec3(0.0f, 0.0f, 0.0f), 10000.0f );

	options3D.mode = DEBUG_RENDER_USE_DEPTH;
	//Make a 3D textured point
	options3D.beginColor = Rgba::BLUE;
	options3D.endColor = Rgba::RED;
	g_debugRenderer->DebugRenderPoint(options3D, Vec3(-10.0f, 0.0f, 0.0f), 20.f, 1.f, m_textureTest);

	options3D.mode = DEBUG_RENDER_XRAY;
	//Make a line in 3D
	options3D.beginColor = Rgba::BLUE;
	options3D.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderLine(options3D, Vec3(0.f, 0.f, 5.f), Vec3(10.f, 0.f, 10.f), 2000.f);

	options3D.mode = DEBUG_RENDER_USE_DEPTH;
	//Make a line in 3D
	options3D.beginColor = Rgba::BLUE;
	options3D.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderLine(options3D, Vec3(0.f, 0.f, 5.f), Vec3(10.f, 0.f, 10.f), 2000.f);

	//Make a sphere
	options3D.beginColor = Rgba::RED;
	options3D.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderSphere(options3D, Vec3(0.f, 3.f, 0.f), 1.f, 10.f, nullptr);
	
	//Make a sphere
	options3D.beginColor = Rgba::GREEN;
	options3D.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderSphere(options3D, Vec3(0.f, -3.f, 0.f), 1.f, 200.f, m_sphereTexture);
	
	//Make a wire sphere
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderWireSphere(options3D, Vec3(0.f, -2.f, 0.f), 1.f, 200.f);
	
	//Make a cube
	options3D.beginColor = Rgba::DARK_GREY;
	options3D.endColor = Rgba::WHITE;
	AABB3 cube = AABB3::UNIT_CUBE;
	g_debugRenderer->DebugRenderBox(options3D, cube, Vec3(-5.f, -1.5f, 0.f), 20.f);

	//Make a wire cube
	options3D.beginColor = Rgba::DARK_GREY;
	options3D.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderWireBox(options3D, cube, Vec3(-5.f, 1.5f, 0.f), 20.f);

	//Make a quad 3D no billboard
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::RED;
	AABB2 quad = AABB2(Vec3(-1.f, -1.f, 0.f), Vec3(1.f, 1.f, 0.f));
	Vec3 position = Vec3(3.f, 2.f, 1.f);
	g_debugRenderer->DebugRenderQuad(options3D, quad, position, 2000.f, m_textureTest, false);

	//Make a quad 3D 
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::RED;
	quad = AABB2(Vec3(-1.f, -1.f, 0.f), Vec3(1.f, 1.f, 0.f));
	position = Vec3(5.f, 2.f, 1.f);
	g_debugRenderer->DebugRenderQuad(options3D, quad, position, 2000.f, m_textureTest);

	//Make text
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::RED;
	const char* text = "This is some text";
	g_debugRenderer->DebugRenderText3D(options3D, Vec3(1.f, 1.f, 1.f), Vec2(1.f, 1.f), text, 0.1f, 20000.f);

	//Make text non billboarded
	options3D.beginColor = Rgba::BLUE;
	options3D.endColor = Rgba::RED;
	const char* textNB = "Billboard this";
	g_debugRenderer->DebugRenderText3D(options3D, Vec3(1.f, 0.5f, 0.f), Vec2(-1.f, 1.f), textNB, 0.2f, 20000.f, false);

	//------------------------------------------------------------------------------------------------------------------------------
	//	LOG Objects
	//------------------------------------------------------------------------------------------------------------------------------
	//Setup Debug Options
	options.mode = DEBUG_RENDER_ALWAYS;
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::YELLOW;
	const char* debugText1 = "Debug Log Test";
	g_debugRenderer->DebugAddToLog(options, debugText1, Rgba::YELLOW, 10.f);

	//Setup Debug Options
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::GREEN;
	const char* debugText2 = "This is another Debug String";
	g_debugRenderer->DebugAddToLog(options, debugText2, Rgba::GREEN, 20.f);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::SetupPhysX(bool isInteractive)
{
	//PhysX starts off by setting up a Physics Foundation
	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);

	//Create the PhysX Visual Debugger by giving it the current foundation
	m_Pvd = PxCreatePvd(*m_PxFoundation);
	//The PVD needs connection via a socket. It will run on the Address defined, in our case it's our machine
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	//Create Physics! This creates an instance of the PhysX SDK
	m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);

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
	m_PxMaterial = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);

	//Add things to your scene
	PxRigidStatic* groundPlane = PxCreatePlane(*m_PhysX, PxPlane(0, 1, 0, 0), *m_PxMaterial);
	m_PxScene->addActor(*groundPlane);
	for (PxU32 i = 0; i < 5; i++)
	{
		CreatePhysXStack(Vec3(0,0,stackZ -= 10.f), 10, 2.f);
	}

	if (!isInteractive)
	{
		CreateDynamicObject(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), Vec3(0, -50, -100));
	}
}

void Game::CreatePhysXStack(const Vec3& position, uint size, float halfExtent)
{
	PxTransform pxTransform = PxTransform(PxVec3(position.x, position.y, position.z));

	//We are going to make a stack of boxes
	PxBoxGeometry box = PxBoxGeometry((PxReal)halfExtent, (PxReal)halfExtent, (PxReal)halfExtent);
	PxShape* shape = m_PhysX->createShape(box, *m_PxMaterial);
	
	//Loop to stack everything in a pyramid shape
	for (PxU32 i = 0; i < size; i++)
	{
		for (PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = m_PhysX->createRigidDynamic(pxTransform.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			m_PxScene->addActor(*body);
		}
	}

	//release the shape now, we don't need it anymore since everything has been added to the PhysX scene
	shape->release();
}

PxRigidDynamic* Game::CreateDynamicObject(const PxTransform& pxTransform, const PxGeometry& pxGeometry, const Vec3& velocity)
{
	PxVec3 pxVelocity = PxVec3(velocity.x, velocity.y, velocity.z);

	PxRigidDynamic* dynamic = PxCreateDynamic(*m_PhysX, pxTransform, pxGeometry, *m_PxMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(pxVelocity);
	m_PxScene->addActor(*dynamic);
	return dynamic;
}

STATIC bool Game::TestEvent(EventArgs& args)
{
	UNUSED(args);
	g_devConsole->PrintString(Rgba::YELLOW, "This a test event called from Game.cpp");
	return true;
}

STATIC bool Game::ToggleLight1( EventArgs& args )
{
	UNUSED(args);
	if(g_renderContext->m_cpuLightBuffer.lights[1].color.a != 0.f)
	{
		g_devConsole->PrintString(Rgba::RED, "Disabling Light 1");
		g_renderContext->m_cpuLightBuffer.lights[1].color.a = 0.f;
	}
	else
	{
		g_devConsole->PrintString(Rgba::GREEN, "Enabling Light 1");
		g_renderContext->m_cpuLightBuffer.lights[1].color.a = 1.f;
	}
	return true;
}

STATIC bool Game::ToggleLight2( EventArgs& args )
{
	UNUSED(args);
	if(g_renderContext->m_cpuLightBuffer.lights[2].color.a != 0.f)
	{
		g_devConsole->PrintString(Rgba::RED, "Disabling Light 2");
		g_renderContext->m_cpuLightBuffer.lights[2].color.a = 0.f;
	}
	else
	{
		g_devConsole->PrintString(Rgba::GREEN, "Enabling Light 2");
		g_renderContext->m_cpuLightBuffer.lights[2].color.a = 1.f;
	}
	return true;
}

STATIC bool Game::ToggleLight3( EventArgs& args )
{
	UNUSED(args);
	if(g_renderContext->m_cpuLightBuffer.lights[3].color.a != 0.f)
	{
		g_devConsole->PrintString(Rgba::RED, "Disabling Light 3");
		g_renderContext->m_cpuLightBuffer.lights[3].color.a = 0.f;
	}
	else
	{
		g_devConsole->PrintString(Rgba::GREEN, "Enabling Light 3");
		g_renderContext->m_cpuLightBuffer.lights[3].color.a = 1.f;
	}
	return true;
}

bool Game::ToggleLight4( EventArgs& args )
{
	UNUSED(args);
	if(g_renderContext->m_cpuLightBuffer.lights[4].color.a != 0.f)
	{
		g_devConsole->PrintString(Rgba::RED, "Disabling Light 4");
		g_renderContext->m_cpuLightBuffer.lights[4].color.a = 0.f;
	}
	else
	{
		g_devConsole->PrintString(Rgba::GREEN, "Enabling Light 4");
		g_renderContext->m_cpuLightBuffer.lights[4].color.a = 1.f;
	}
	return true;
}

STATIC bool Game::ToggleAllPointLights( EventArgs& args )
{
	UNUSED(args);
	for(int i = 1; i < 5; i++)
	{
		if(g_renderContext->m_cpuLightBuffer.lights[i].color.a != 0.f)
		{
			g_renderContext->m_cpuLightBuffer.lights[i].color.a = 0.f;
		}
		else
		{
			g_renderContext->m_cpuLightBuffer.lights[i].color.a = 1.f;
		}
	}
	g_devConsole->PrintString(Rgba::GREEN, "Toggled All Point Lights");
	return true;
}

void Game::HandleKeyPressed(unsigned char keyCode)
{
	if(g_devConsole->IsOpen())
	{
		g_devConsole->HandleKeyDown(keyCode);
		return;
	}

	switch( keyCode )
	{
		case UP_ARROW:
		{
			//Increase emissive factor
			m_emissiveFactor += m_emissiveStep;
		}
		break;
		case DOWN_ARROW:
		{
			//decrease emissive factor
			m_emissiveFactor -= m_emissiveStep;
		}
		break;
		case RIGHT_ARROW:
		case LEFT_ARROW:
		case SPACE_KEY:
		case N_KEY:
		case F1_KEY:
		{
			//Setup the ambient intensity to lesser
			m_ambientIntensity -= m_ambientStep;
		}
		break;
		case F2_KEY:
		{
			//Setup the ambient intensity to lesser
			m_ambientIntensity += m_ambientStep;
		}
		break;
		case F3_KEY:
		{
			//Toggle directional light
			m_enableDirectional = !m_enableDirectional;
		}
		break;
		case A_KEY:
		{
			//Handle left movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetIVector() * -1.f;
			worldMovementDirection *= (m_cameraSpeed);

			m_camPosition += worldMovementDirection; 
		}
		break;
		case W_KEY:
		{
			//Handle forward movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetKVector();
			worldMovementDirection *= (m_cameraSpeed); 

			m_camPosition += worldMovementDirection; 
		}
		break;
		case S_KEY:
		{
			//Handle backward movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetKVector() * -1.f;
			worldMovementDirection *= (m_cameraSpeed); 

			m_camPosition += worldMovementDirection; 
		}
		break;
		case D_KEY:
		{
			//Handle right movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetIVector();
			worldMovementDirection *= (m_cameraSpeed); 

			m_camPosition += worldMovementDirection; 
		}
		break;
		case F4_KEY:
		{
			//Set volume back to 1
			//g_audio->SetSoundPlaybackVolume(m_testPlayback, 1.0f);

			//Toggle Shader here
			m_normalMode = !m_normalMode;

			break;
		}
		case F5_KEY:
		{
			//Set volume to 0
			//g_audio->SetSoundPlaybackVolume(m_testPlayback, 0.0f);
			
			//toggle material or not
			m_useMaterial = !m_useMaterial;

			break;
		}
		case F6_KEY:
		{
			//Fire event
			g_eventSystem->FireEvent("TestEvent");
			break;
		}		case F7_KEY:
		{
			//Quit Debug
			g_eventSystem->FireEvent("Quit");
			break;
		}
		default:
		break;
	}
}

//Function that handles debug mode enabled
void Game::DebugEnabled()
{
	g_debugMode = !g_debugMode;
}


void Game::Shutdown()
{
	delete m_mainCamera;
	m_mainCamera = nullptr;

	delete m_devConsoleCamera;
	m_devConsoleCamera = nullptr;

	delete m_cube;
	m_cube = nullptr;

	delete m_sphere;
	m_sphere = nullptr;

	delete m_quad;
	m_quad = nullptr;

	delete m_baseQuad;
	m_baseQuad = nullptr;

	delete m_capsule;
	m_capsule = nullptr;

	PhysXShutdown();
	//FreeResources();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::PhysXShutdown()
{
	//Handle all shutdown code here for Nvidia PhysX
	PX_RELEASE(m_PxScene);
	PX_RELEASE(m_PxDispatcher);
	PX_RELEASE(m_PhysX);
	if (m_Pvd)
	{
		PxPvdTransport* transport = m_Pvd->getTransport();
		m_Pvd->release();	m_Pvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(m_PxFoundation);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::HandleKeyReleased(unsigned char keyCode)
{
	if(g_devConsole->IsOpen())
	{
		g_devConsole->HandleKeyUp(keyCode);
		return;
	}

	//SoundID testSound = g_audio->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
	switch( keyCode )
	{
		case UP_ARROW:
		case RIGHT_ARROW:
		case LEFT_ARROW:
		//g_audio->PlaySound( m_testAudioID );
		break;
		default:
		break;
	}
}

void Game::HandleCharacter( unsigned char charCode )
{
	if(g_devConsole->IsOpen())
	{
		g_devConsole->HandleCharacter(charCode);
		return;
	}
}

void Game::EnablePointLight( uint slot, const Vec3& position, const Vec3& direction, const Rgba& color /*= Rgba::WHITE*/, float intensity /*= 1.f*/, const Vec3& diffuseAttenuation, const Vec3& specularAttenuation ) const
{
	LightT pointLight;

	pointLight.position = position;
	pointLight.color = color;
	pointLight.color.a = intensity;
	pointLight.direction = direction;
	pointLight.diffuseAttenuation = diffuseAttenuation;
	pointLight.specularAttenuation = specularAttenuation;

	g_renderContext->EnableLight(slot, pointLight);
}

void Game::EnableDirectionalLight( const Vec3& position, const Vec3& direction,  const Rgba& color /*= Rgba::WHITE*/, float intensity /*= 1.f*/, const Vec3& diffuseAttenuation, const Vec3& specularAttenuation) const
{
	LightT directionalLight;

	directionalLight.position = position;
	directionalLight.color = color;
	directionalLight.color.a = intensity;
	directionalLight.direction = direction;
	directionalLight.isDirection = 1.f;
	directionalLight.diffuseAttenuation = diffuseAttenuation;
	directionalLight.specularAttenuation = specularAttenuation;

	g_renderContext->EnableLight(0U, directionalLight);
}

void Game::Render() const
{
	//Get the ColorTargetView from rendercontext
	ColorTargetView *colorTargetView = g_renderContext->GetFrameColorTarget();

	//Setup what we are rendering to
	m_mainCamera->SetColorTarget(colorTargetView);
	m_devConsoleCamera->SetColorTarget(colorTargetView);

	// Move the camera to where it is in the scene
	Matrix44 camTransform = Matrix44::MakeFromEuler( m_mainCamera->GetEuler(), m_rotationOrder ); 
	camTransform = Matrix44::SetTranslation3D(m_camPosition, camTransform);
	m_mainCamera->SetModelMatrix(camTransform);

	g_renderContext->BeginCamera(*m_mainCamera); 
	
	g_renderContext->ClearColorTargets(Rgba(ui_testColor[0], ui_testColor[1], ui_testColor[2], 1.f));

	float intensity = Clamp(m_ambientIntensity, 0.f, 1.f);
	g_renderContext->SetAmbientLight( Rgba::WHITE, intensity ); 

	float emissive = Clamp(m_emissiveFactor, 0.1f, 1.f);
	g_renderContext->m_cpuLightBuffer.emissiveFactor = emissive;

	// enable a point light as some position in the world with a normal quadratic falloff; 
	if(m_enableDirectional)
	{
		g_renderContext->DisableDirectionalLight();
	}
	else 
	{
		g_renderContext->EnableDirectionalLight();
	}

	//RenderUsingMaterial();
	RenderIsoSprite();
	RenderPhysXScene();

	g_renderContext->EndCamera();

	/*
	//Render the Quad
	g_renderContext->BindTextureViewWithSampler(0U, nullptr);
	g_renderContext->SetModelMatrix(m_baseQuadTransform);
	g_renderContext->DrawMesh( m_baseQuad );	
	*/

	if(!m_consoleDebugOnce)
	{
		EventArgs* args = new EventArgs();
		std::string key = "TestString";
		std::string value = "This is a test";
		args->SetValue(key, value);
		g_devConsole->Command_Test(*args);
		g_devConsole->ExecuteCommandLine("Exec Health=25");
		g_devConsole->ExecuteCommandLine("Exec Health=85 Armor=100");
	}

	//Uncomment to get Debug Rendering to work
	DebugRenderToCamera();

	if(g_devConsole->IsOpen())
	{	
		g_devConsole->Render(*g_renderContext, *m_devConsoleCamera, DEVCONSOLE_LINE_HEIGHT);
	}	

}

void Game::RenderUsingMaterial() const
{
	g_renderContext->BindMaterial(m_testMaterial);

	//Render the cube
	//g_renderContext->BindTextureViewWithSampler(0U, m_boxTexturePath);  
	g_renderContext->SetModelMatrix(m_cubeTransform);
	g_renderContext->DrawMesh( m_cube ); 

	//Render the sphere
	//g_renderContext->BindTextureViewWithSampler(0U, m_sphereTexturePath); 
	g_renderContext->SetModelMatrix( m_sphereTransform ); 
	g_renderContext->DrawMesh( m_sphere ); 

	//Render the Quad
	//g_renderContext->BindTextureViewWithSampler(0U, nullptr);
	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);
	g_renderContext->DrawMesh( m_quad );

	//Render the capsule here
	g_renderContext->SetModelMatrix(m_capsuleModel);
	g_renderContext->DrawMesh(m_capsule);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderPhysXScene() const
{
	//Get the actors in the scene
	PxScene* scene;
	PxGetPhysics().getScenes(&scene, 1);

	//Bind Material
	g_renderContext->BindMaterial(m_testMaterial);

	PxU32 numActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (numActors)
	{
		std::vector<PxRigidActor*> actors(numActors);
		scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), numActors);

		RenderPhysXActors(actors, (int)actors.size());
	}
}

void Game::RenderPhysXActors(const std::vector<PxRigidActor*> actors, int numActors, const Rgba& color) const
{
	//Look for maximum of 3 shapes to draw per actor
	PxShape* shapes[3];

	for (int i = 0; i < numActors; i++)
	{
		const PxU32 numShapes = actors[i]->getNbShapes();
		actors[i]->getShapes(shapes, numShapes);

		const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

		for (PxU32 j = 0; j < numShapes; j++)
		{
			int type = shapes[j]->getGeometryType();

			switch (type)
			{
			case PxGeometryType::eBOX:
			{
				PxBoxGeometry box;
				shapes[j]->getBoxGeometry(box);
				Vec3 halfExtents = PxVectorToVec(box.halfExtents);

				CPUMesh mesh;
				CPUMeshAddCube(&mesh, AABB3(-1.f * halfExtents, halfExtents), color);
				m_cube->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

				PxMat44 pxTransform = actors[i]->getGlobalPose();
				PxVec3 pxPosition = pxTransform.getPosition();

				Matrix44 pose;
				pose.SetIVector(PxVectorToVec(pxTransform.column0));
				pose.SetJVector(PxVectorToVec(pxTransform.column1));
				pose.SetKVector(PxVectorToVec(pxTransform.column2));
				pose.SetTVector(PxVectorToVec(pxTransform.column3));

				Matrix44 position;
				position.SetTranslation3D(PxVectorToVec(pxPosition), position);
				position.SetRotationFromMatrix(position, pose);
				

				g_renderContext->SetModelMatrix(position);
				g_renderContext->DrawMesh(m_cube);
			}
			break;
			default:
				break;
			}

		}
	}
}

void Game::DebugRenderToScreen() const
{
	Camera& debugCamera = g_debugRenderer->Get2DCamera();
	debugCamera.m_colorTargetView = g_renderContext->GetFrameColorTarget();
	
	g_renderContext->BindShader(m_shader);
	g_renderContext->BeginCamera(debugCamera);
	
	g_debugRenderer->DebugRenderToScreen();

	g_renderContext->EndCamera();
	
}

void Game::DebugRenderToCamera() const
{
	Camera& debugCamera3D = *m_mainCamera;
	debugCamera3D.m_colorTargetView = g_renderContext->GetFrameColorTarget();

	g_renderContext->BindShader(m_shader);

	g_renderContext->BeginCamera(debugCamera3D);
	
	g_debugRenderer->Setup3DCamera(&debugCamera3D);
	g_debugRenderer->DebugRenderToCamera();

	g_renderContext->EndCamera();
}

void Game::PostRender()
{
	//Debug bools
	m_consoleDebugOnce = true;

	if(!m_isDebugSetup)
	{
		//SetStartupDebugRenderObjects();

		ColorTargetView* ctv = g_renderContext->GetFrameColorTarget();
		//Setup debug render client data
		g_debugRenderer->SetClientDimensions( ctv->m_height, ctv->m_width );

		m_isDebugSetup = true;
	}

	//All screen Debug information
	DebugRenderToScreen();

	g_ImGUI->Render();
}

void Game::Update( float deltaTime )
{
	UpdatePhysX(deltaTime);

	UpdateLightPositions();

	//Figure out update state for only move on alt + move
	UpdateMouseInputs(deltaTime);

	g_ImGUI->BeginFrame();

	if(g_devConsole->GetFrameCount() > 1 && !m_devConsoleSetup)
	{
		m_devConsoleCamera->SetOrthoView(Vec2(-WORLD_WIDTH * 0.5f * SCREEN_ASPECT, -WORLD_HEIGHT * 0.5f), Vec2(WORLD_WIDTH * 0.5f * SCREEN_ASPECT, WORLD_HEIGHT * 0.5f));
		m_devConsoleSetup = true;
	}

	//UpdateCamera(deltaTime);
	g_renderContext->m_frameCount++;

	CheckXboxInputs();
	m_animTime += deltaTime;

	DebugRenderOptionsT options;
	float currentTime = static_cast<float>(GetCurrentTimeSeconds());
	const char* text = "Current Time %f";
	
	g_debugRenderer->DebugAddToLog(options, text, Rgba::YELLOW, 0.f, currentTime);

	text = "F5 to Toggle Material/Legacy mode";
	g_debugRenderer->DebugAddToLog(options, text, Rgba::WHITE, 0.f);
	
	text = "UP/DOWN to increase/decrease emissive factor";
	g_debugRenderer->DebugAddToLog(options, text, Rgba::WHITE, 0.f);

	//Update the camera's transform
	Matrix44 camTransform = Matrix44::MakeFromEuler( m_mainCamera->GetEuler(), m_rotationOrder ); 
	camTransform = Matrix44::SetTranslation3D(m_camPosition, camTransform);
	m_mainCamera->SetModelMatrix(camTransform);
	
	//float currentTime = static_cast<float>(GetCurrentTimeSeconds());

	// Set the cube to rotate around y (which is up currently),
	// and move the object to the left by 5 units (-x)
	//m_cubeTransform = Matrix44::MakeFromEuler( Vec3(60.0f * currentTime, 0.0f, 0.0f), m_rotationOrder ); 
	m_cubeTransform = Matrix44::SetTranslation3D( Vec3(-5.0f, 0.0f, 0.0f), m_cubeTransform);

	m_sphereTransform = Matrix44::MakeFromEuler( Vec3(0.0f, -45.0f * currentTime, 0.0f) ); 
	m_sphereTransform = Matrix44::SetTranslation3D( Vec3(5.0f, 0.0f, 0.0f), m_sphereTransform);

	m_quadTransfrom = Matrix44::SetTranslation3D(Vec3(0.f, 0.f, 0.f), m_quadTransfrom);

	//g_debugRenderer->DebugRenderPoint(Vec3(0.f, 0.f, 0.f), 0.f, 1.f);

	m_testDirection = m_testDirection.GetRotatedAboutYDegrees(currentTime * ui_testSlider);

	CheckCollisions();

	ClearGarbageEntities();	

	UpdateImGUI();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::UpdatePhysX(float deltaTime)
{
	m_PxScene->simulate(deltaTime);
	m_PxScene->fetchResults(true);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::UpdateImGUI()
{
	//Use this place to create/update info for imGui
	ImGui::NewFrame();

	//float value = 0.5f;
	//float color[3] = { 0.f, 1.f, 1.f };
	//bool show = true;

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &ui_testCheck1);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &ui_testCheck2);

	ImGui::SliderFloat("float", &ui_testSlider, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&ui_testColor); // Edit 3 floats representing a color

	ImGui::End();
}

//Use this chunk of code only for screen shake!
/*
void Game::UpdateCamera(float deltaTime)
{
	g_mainCamera = new Camera();
	Vec2 orthoBottomLeft = Vec2(0.f,0.f);
	Vec2 orthoTopRight = Vec2(WORLD_WIDTH, WORLD_HEIGHT);
	g_mainCamera->SetOrthoView(orthoBottomLeft, orthoTopRight);

	float shakeX = 0.f;
	float shakeY = 0.f;

	if(g_shakeAmount > 0)
	{
		shakeX = g_randomNumGen->GetRandomFloatInRange(-g_shakeAmount, g_shakeAmount);
		shakeY = g_randomNumGen->GetRandomFloatInRange(-g_shakeAmount, g_shakeAmount);

		g_shakeAmount -= deltaTime * CAMERA_SHAKE_REDUCTION_PER_SECOND;
	}
	else
	{
		g_shakeAmount = 0;
	}

	Vec2 translate2D = Vec2(shakeX, shakeY);
	translate2D.ClampLength(MAX_SHAKE);
	g_mainCamera->Translate2D(translate2D);
}
*/

void Game::ClearGarbageEntities()
{
}

void Game::CheckXboxInputs()
{
	//XboxController playerController = g_inputSystem->GetXboxController(0);
}

void Game::CheckCollisions()
{		
}

//------------------------------------------------------------------------------------------------------------------------------
Vec3 Game::PxVectorToVec(const PxVec3& pxVector) const
{
	Vec3 vector;
	vector.x = pxVector.x;
	vector.y = pxVector.y;
	vector.z = pxVector.z;

	return vector;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec4 Game::PxVectorToVec(const PxVec4& pxVector) const
{
	Vec4 vector;
	vector.x = pxVector.x;
	vector.y = pxVector.y;
	vector.z = pxVector.z;
	vector.w = pxVector.w;

	return vector;
}

bool Game::IsAlive()
{
	//Check if alive
	return m_isGameAlive;
}

void Game::LoadGameMaterials()
{
	m_testMaterial = g_renderContext->CreateOrGetMaterialFromFile(m_materialPath);
}

void Game::UpdateLightPositions()
{
	//Update all the 4 light positions
	float currentTime = static_cast<float>(GetCurrentTimeSeconds());
	DebugRenderOptionsT options;
	options.space = DEBUG_RENDER_WORLD;
	//Light 1
	m_dynamicLight0Pos = Vec3(-3.f, 2.f * CosDegrees(currentTime * 20.f), 2.f * SinDegrees(currentTime * 20.f));

	g_renderContext->m_cpuLightBuffer.lights[1].position = m_dynamicLight0Pos;
	g_renderContext->m_lightBufferDirty = true;

	options.beginColor = Rgba::GREEN;
	options.endColor = Rgba::GREEN * 0.4f;
	g_debugRenderer->DebugRenderPoint(options, m_dynamicLight0Pos, 0.1f, 0.1f, nullptr);

	//Light 2
	m_dynamicLight1Pos = Vec3(3.f, 3.f * CosDegrees(currentTime * 40.f), 3.f * SinDegrees(currentTime * 40.f));
	g_renderContext->m_cpuLightBuffer.lights[2].position = m_dynamicLight1Pos;
	g_renderContext->m_lightBufferDirty = true;

	options.beginColor = Rgba::BLUE;
	options.endColor = Rgba::BLUE * 0.4f;
	g_debugRenderer->DebugRenderPoint(options, m_dynamicLight1Pos, 0.1f, 0.1f, nullptr);

	//Light 3
	m_dynamicLight2Pos = Vec3(-1.f, 1.f * CosDegrees(currentTime * 30.f), 1.f * SinDegrees(currentTime * 30.f));
	g_renderContext->m_cpuLightBuffer.lights[3].position = m_dynamicLight2Pos;
	g_renderContext->m_lightBufferDirty = true;

	options.beginColor = Rgba::YELLOW;
	options.endColor = Rgba::YELLOW * 0.4f;
	g_debugRenderer->DebugRenderPoint(options, m_dynamicLight2Pos, 0.1f, 0.1f, nullptr);

	//Light 4
	m_dynamicLight3Pos = Vec3(4.f * CosDegrees(currentTime * 60.f), 0.f , 4.f * SinDegrees(currentTime * 60.f));
	g_renderContext->m_cpuLightBuffer.lights[4].position = m_dynamicLight3Pos;
	g_renderContext->m_lightBufferDirty = true;

	options.beginColor = Rgba::MAGENTA;
	options.endColor = Rgba::MAGENTA * 0.4f;
	g_debugRenderer->DebugRenderPoint(options, m_dynamicLight3Pos, 0.1f, 0.1f, nullptr);
}

void Game::RenderIsoSprite() const
{
	Vec2 right = Vec2(1.f, 0.f);
	Vec2 up = Vec2(0.f, 1.f);

	Vec3 mins = Vec3(-m_quadSize * 0.5f, -m_quadSize * 0.5f, 0.f);
	Vec3 maxs = Vec3(m_quadSize * 0.5f, m_quadSize * 0.5f, 0.f);

	AABB2 box = AABB2(mins, maxs);

	//SpriteDefenition def = m_isoSprite->GetSpriteForLocalDirection(m_testDirection);

	//Vec2 spriteMins;
	//Vec2 spriteMaxs;
	/*def->GetUVs(spriteMins, spriteMaxs);*/

	//g_renderContext->BindMaterial(m_testMaterial);
// 	spriteMins.y = 1 - spriteMins.y;
// 	spriteMaxs.y = 1 - spriteMaxs.y;
	
	CPUMesh mesh;
	CPUMeshAddQuad(&mesh, AABB2(Vec2(-0.5f, -0.5f), Vec2(0.5f, 0.5f)), Rgba::WHITE);
	m_quad->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	g_renderContext->BindShader(g_renderContext->CreateOrGetShaderFromFile("default_unlit.xml"));
	//TextureView* view = def->GetTexture();
	TextureView* view = m_laborerSheet;
	g_renderContext->BindTextureView(0U, view);
	g_renderContext->SetModelMatrix(m_quadTransfrom);

	g_renderContext->DrawMesh(m_quad);

	g_renderContext->BindTextureView(0U, nullptr);
}

void Game::CreateInitialLight()
{
	EnableDirectionalLight(Vec3(1.f, 1.f, 1.f), Vec3(0.f, 0.f, 1.f));

	EnablePointLight(1U, m_dynamicLight0Pos, Vec3(1.f, 0.f, 0.5f),Rgba::GREEN);
	EnablePointLight(2U, m_dynamicLight1Pos, Vec3(0.f, -1.f, 0.f), Rgba::BLUE, 1.f, Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.1f, 0.f));
	EnablePointLight(3U, m_dynamicLight2Pos, Vec3(0.f, 0.f, 1.f), Rgba::YELLOW, 1.f, Vec3(0.f, 1.f, 0.1f), Vec3(0.f, 0.1f, 0.f));
	EnablePointLight(4U, m_dynamicLight3Pos, Vec3(-1.f, -1.f, 0.f), Rgba::MAGENTA, 1.f, Vec3(0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f));
}

void Game::CreateInitialMeshes()
{

	//Meshes for A4
	CPUMesh mesh;
	CPUMeshAddQuad(&mesh, AABB2(Vec2(-0.5f, -0.5f), Vec2(0.5f, 0.5f)));
	m_quad = new GPUMesh(g_renderContext);
	m_quad->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	// create a cube (centered at zero, with sides 1 length)
	CPUMeshAddCube( &mesh, AABB3( Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f)) ); 
	
	//mesh.SetLayout<Vertex_Lit>();
	m_cube = new GPUMesh( g_renderContext ); 
	m_cube->CreateFromCPUMesh<Vertex_Lit>( &mesh, GPU_MEMORY_USAGE_STATIC );


	// create a sphere, cenetered at zero, with 
	mesh.Clear();
	CPUMeshAddUVSphere( &mesh, Vec3::ZERO, 1.0f );  
	
	//mesh.SetLayout<Vertex_Lit>();
	m_sphere = new GPUMesh( g_renderContext ); 
	m_sphere->CreateFromCPUMesh<Vertex_Lit>( &mesh, GPU_MEMORY_USAGE_STATIC );

	//Create another quad as a base plane
	mesh.Clear();
	CPUMeshAddQuad(&mesh, AABB2(Vec2(-50.f, -50.f), Vec2(50.f, 50.f)));

	//mesh.SetLayout<Vertex_Lit>();
	m_baseQuad = new GPUMesh( g_renderContext ); 
	m_baseQuad->CreateFromCPUMesh<Vertex_Lit>( &mesh, GPU_MEMORY_USAGE_STATIC );

	m_baseQuadTransform = Matrix44::IDENTITY;
	m_baseQuadTransform = Matrix44::MakeFromEuler(Vec3(-90.f, 0.f, 0.f));
	m_baseQuadTransform = Matrix44::SetTranslation3D(Vec3(0.f, -1.f, 0.f), m_baseQuadTransform);

	mesh.Clear();
	CPUMeshAddUVCapsule(&mesh, Vec3(0.f, 1.f, 1.f), Vec3(0.f, -1.f, 1.f), 2.f, Rgba::YELLOW);

	m_capsule = new GPUMesh(g_renderContext);
	m_capsule->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	m_capsuleModel = Matrix44::IDENTITY;
	m_capsuleModel = Matrix44::MakeFromEuler(Vec3(-90.f, 0.f, 0.f));
}

void Game::LoadGameTextures()
{
	//Get the test texture
	m_textureTest = g_renderContext->CreateOrGetTextureViewFromFile(m_testImagePath);
	m_boxTexture = g_renderContext->CreateOrGetTextureViewFromFile(m_boxTexturePath);
	m_sphereTexture = g_renderContext->CreateOrGetTextureViewFromFile(m_sphereTexturePath);

	//Load the sprite sheet from texture (Need to do XML test)
	m_laborerSheet = g_renderContext->CreateOrGetTextureViewFromFile(m_laborerSheetPath);
	m_testSheet = new SpriteSheet(m_laborerSheet, m_laborerSheetDim);

	CreateIsoSpriteDefenitions();
}

void Game::CreateIsoSpriteDefenitions()
{	
	std::vector<SpriteDefenition> spriteDefs;
	std::vector<Vec3> directions;

	Vec3 dir;

	spriteDefs.push_back(SpriteDefenition(m_testSheet->GetSpriteDef(0), Vec2(0.5, 0.25)));
	directions.push_back(Vec3(0.f, 0.f, 1.f));

	spriteDefs.push_back(SpriteDefenition(m_testSheet->GetSpriteDef(16), Vec2(0.5, 0.25)));
	dir = Vec3(-1.f, 0.f, 1.f).GetNormalized();
	directions.push_back(dir);

	spriteDefs.push_back(SpriteDefenition(m_testSheet->GetSpriteDef(32), Vec2(0.5, 0.25)));
	directions.push_back(Vec3(-1.f, 0.f, 0.f));

	spriteDefs.push_back(SpriteDefenition(m_testSheet->GetSpriteDef(48), Vec2(0.5, 0.25)));
	dir = Vec3(-1.f, 0.f, -1.f).GetNormalized();
	directions.push_back(dir);

	spriteDefs.push_back(SpriteDefenition(m_testSheet->GetSpriteDef(64), Vec2(0.5, 0.25)));
	directions.push_back(Vec3(0.f, 0.f, -1.f));

	spriteDefs.push_back(SpriteDefenition(m_testSheet->GetSpriteDef(80), Vec2(0.5, 0.25)));
	dir = Vec3(1.f, 0.f, -1.f).GetNormalized();
	directions.push_back(dir);

	spriteDefs.push_back(SpriteDefenition(m_testSheet->GetSpriteDef(96), Vec2(0.5, 0.25)));
	directions.push_back(Vec3(1.f, 0.f, 0.f));
	
//	m_isoSprite = new IsoSpriteDefenition(&spriteDefs[0], &directions[0], 7);
}

void Game::GetandSetShaders()
{
	//Get the Shader
	m_shader = g_renderContext->CreateOrGetShaderFromFile(m_xmlShaderPath);
	m_shader->SetDepth(eCompareOp::COMPARE_LEQUAL, true);

	m_normalShader = g_renderContext->CreateOrGetShaderFromFile(m_normalColorShader);
	m_normalShader->SetDepth(eCompareOp::COMPARE_LEQUAL, true);

	m_defaultLit = g_renderContext->CreateOrGetShaderFromFile(m_shaderLitPath);
	m_defaultLit->SetDepth(eCompareOp::COMPARE_LEQUAL, true);
}

void Game::UpdateMouseInputs(float deltaTime)
{
	//Get pitch and yaw from mouse
	IntVec2 mouseRelativePos = g_windowContext->GetClientMouseRelativeMovement();
	Vec2 mouse = Vec2((float)mouseRelativePos.x, (float)mouseRelativePos.y);

	// we usually want to scale the pixels so we can think of it
	// as a rotational velocity;  Work with these numbers until 
	// it feels good to you; 
	Vec2 scalingFactor = Vec2( 10.f, 10.f ); 
	Vec2 turnSpeed = mouse * scalingFactor; 

	// y mouse movement would corresond to rotation around right (x for us)
	// and x mouse movement corresponds with movement around up (y for us)
	Vec3 camEuler = m_mainCamera->GetEuler();
	camEuler -= deltaTime * Vec3( turnSpeed.y, turnSpeed.x, 0.0f ); 
	m_mainCamera->SetEuler(camEuler);

	// Let's fix our "pitch", or rotation around right to be limited to -85 to 85 degrees (no going upside down)
	camEuler.x = Clamp( camEuler.x, -85.0f, 85.0f );

	// Next, let's keep the turning as between 0 and 360 (some people prefer -180.0f to 180.0f)
	// either way, we just want to keep it a single revolution
	// Note: modf does not correctly mod negative numbers (it'll ignore the sign and mod them as if 
	// they were positive), so I write a special mod function to take this into account;  
	//m_camEuler.y = Modf( m_camEuler.y, 360.0f ); 

	// Awesome, I now have my euler, let's construct a matrix for it;
	// this gives us our current camera's orientation.  we will 
	// use this to translate our local movement to a world movement 
	Matrix44 camMatrix = Matrix44::MakeFromEuler( camEuler ); 

	//Test implementation
	//m_camEuler.y -= static_cast<float>(mouseRelativePos.x);
	//m_camEuler.x -= static_cast<float>(mouseRelativePos.y);


}
