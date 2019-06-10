//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/IsoSpriteDefenition.hpp"
//Game Systems
#include "Game/GameCommon.hpp"
//Third Party
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "PxFoundation.h"
#include "pvd/PxPvd.h"
#include "PxRigidDynamic.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

using namespace physx; 

//------------------------------------------------------------------------------------------------------------------------------
class Texture;
class BitmapFont;
class TextureView;
class Image;
class Shader;
class SpriteAnimDefenition;
class GPUMesh;

struct Camera;

//------------------------------------------------------------------------------------------------------------------------------
class Game
{
public:
	Game();
	~Game();
	
	static bool TestEvent(EventArgs& args);
	static bool ToggleLight1(EventArgs& args);
	static bool ToggleLight2(EventArgs& args);
	static bool ToggleLight3(EventArgs& args);
	static bool ToggleLight4(EventArgs& args);
	static bool ToggleAllPointLights(EventArgs& args);

	void								StartUp();
	
	void								SetupMouseData();
	void								SetupCameras();
	void								GetandSetShaders();
	void								LoadGameTextures();
	void								CreateIsoSpriteDefenitions();
	void								LoadGameMaterials();
	void								CreateInitialMeshes();
	void								CreateInitialLight();
	void								SetStartupDebugRenderObjects();
	void								SetupPhysX(bool isInteractive);

	void								CreatePhysXStack(const Vec3& position, uint size, float halfExtent);
	PxRigidDynamic*						CreateDynamicObject(const PxGeometry& pxGeometry, const Vec3& velocity);

	void								HandleKeyPressed(unsigned char keyCode);
	void								HandleKeyReleased( unsigned char keyCode );
	void								HandleCharacter( unsigned char charCode );

	void								EnablePointLight( uint slot, const Vec3& position, const Vec3& direction,
															const Rgba& color = Rgba::WHITE, float intensity = 1.f,
															const Vec3& diffuseAttenuation = Vec3(1.f, 0.f, 0.f),
															const Vec3& specularAttenuation = Vec3(1.f, 0.f, 0.f)) const;
	
	void								EnableDirectionalLight( const Vec3& position, const Vec3& direction,
															const Rgba& color = Rgba::WHITE, float intensity = 1.f,
															const Vec3& diffuseAttenuation = Vec3(1.f, 0.f, 0.f),
															const Vec3& specularAttenuation = Vec3(1.f, 0.f, 0.f)) const;

	void								DebugEnabled();
	void								Shutdown();
	void								PhysXShutdown();

	void								Render() const;
	void								RenderUsingMaterial() const;
	void								RenderPhysXScene() const;
	void								RenderPhysXActors(const std::vector<PxRigidActor*> actors, int numActors, const Rgba& color = Rgba::WHITE) const;
	void								RenderIsoSprite() const;
	void								DebugRenderToScreen() const;
	void								DebugRenderToCamera() const;
	void								PostRender();
	void								Update( float deltaTime );
	void								UpdatePhysX( float deltaTime );
	void								UpdateImGUI();
	void								UpdateMouseInputs(float deltaTime);
	void								UpdateLightPositions();
	void								UpdateCamera(float deltaTime);
	void								ClearGarbageEntities();
	void								CheckXboxInputs();
	void								CheckCollisions();

	Vec3								PxVectorToVec(const PxVec3& pxVector) const; 
	Vec4								PxVectorToVec(const PxVec4& pxVector) const;

	bool								IsAlive();
private:
	bool								m_isGameAlive = false;
	bool								m_consoleDebugOnce = false;
	bool								m_devConsoleSetup = false;
	bool								m_isDebugSetup = false;
	float								m_cameraSpeed = 0.3f; 

public:
	SoundID								m_testAudioID = NULL;
	
	TextureView*						m_textureTest = nullptr;
	TextureView*						m_boxTexture = nullptr;
	TextureView*						m_sphereTexture = nullptr;
	BitmapFont*							m_squirrelFont = nullptr;
	Image*								m_testImage = nullptr;
	float								m_animTime = 0.f;

	//D3D11 stuff
	Shader*								m_shader = nullptr;
	Shader*								m_normalShader = nullptr;
	Shader*								m_defaultLit = nullptr;
	std::string							m_defaultShaderPath = "default_unlit.00.hlsl";
	std::string							m_shaderLitPath = "default_lit_PCUN.hlsl";
	std::string							m_normalColorShader = "normal_shader.hlsl";
	std::string							m_testImagePath = "Test_StbiFlippedAndOpenGL.png";
	std::string							m_boxTexturePath = "woodcrate.jpg";
	std::string							m_sphereTexturePath = "2k_earth_daymap.jpg";
	std::string							m_xmlShaderPath = "default_unlit.xml";
	std::string							m_materialPath = "couch.mat";

	Camera*								m_mainCamera = nullptr;
	Camera*								m_devConsoleCamera = nullptr;
	Rgba*								m_clearScreenColor = nullptr;
	
	float								m_camFOVDegrees = 60.f; //Desired Camera Field of View
	eRotationOrder						m_rotationOrder = ROTATION_ORDER_DEFAULT;
	Vec3								m_camPosition = Vec3::ZERO;

	float								m_devConsoleScreenWidth = 0.f;
	float								m_devConsoleScreenHeight = 0.f;

	//FOR ASSIGNMENT 4:
	// Define the shapes, and how are they positionedin the world; 
	GPUMesh*							m_cube = nullptr; 
	Matrix44							m_cubeTransform; // cube's model matrix

	GPUMesh*							m_sphere = nullptr;
	Matrix44							m_sphereTransform;   // sphere's model matrix

	GPUMesh*							m_quad = nullptr;
	Matrix44							m_quadTransfrom;

	GPUMesh*							m_baseQuad = nullptr;
	Matrix44							m_baseQuadTransform;

	GPUMesh*							m_capsule = nullptr;
	Matrix44							m_capsuleModel;

	//Lighting Assignment
	int									m_lightSlot;
	float								m_ambientIntensity = 1.f;
	float								m_ambientStep = 0.1f;

	bool								m_enableDirectional = true;
	bool								m_normalMode = false;

	//Light positions
	Vec3								m_dynamicLight0Pos = Vec3::ZERO;
	Vec3								m_dynamicLight1Pos = Vec3::ZERO;
	Vec3								m_dynamicLight2Pos = Vec3::ZERO;
	Vec3								m_dynamicLight3Pos = Vec3::ZERO;
	
	//Light movement
	float								m_ySpeed = 2.f;

	//Material
	Material*							m_testMaterial = nullptr;
	bool								m_useMaterial = true;

	float								m_emissiveFactor = 0.f;
	float								m_emissiveStep = 0.1f;

	//------------------------------------------------------------------------------------------------------------------------------
	// User Interface Variables
	//------------------------------------------------------------------------------------------------------------------------------

	float								ui_testSlider = 0.5f;
	float								ui_testColor[3] = { 0.f, 0.f, 0.f };
	bool								ui_testCheck1 = false;
	bool								ui_testCheck2 = true;


	//------------------------------------------------------------------------------------------------------------------------------
	// PhysX Test Variables
	//------------------------------------------------------------------------------------------------------------------------------

	//NxPhysicsSDK*						pPhysics; //declare globally
	PxDefaultAllocator					m_PxAllocator;
	PxDefaultErrorCallback				m_PXErrorCallback;

	PxFoundation*						m_PxFoundation = NULL;
	PxPhysics*							m_PhysX = NULL;

	PxDefaultCpuDispatcher*				m_PxDispatcher = NULL;
	PxScene*							m_PxScene = NULL;

	PxMaterial*							m_PxMaterial = NULL;

	PxPvd*								m_Pvd = NULL;

	PxReal								stackZ = 10.0f;


	//------------------------------------------------------------------------------------------------------------------------------
	// Iso Sprite Test Variables
	//------------------------------------------------------------------------------------------------------------------------------

	Vec2								m_position = Vec2::ZERO;
	Vec2								m_targetPosition = Vec2::ZERO;
	std::string							m_laborerSheetPath = "Laborer_spriteshee_2k.png";
	TextureView*						m_laborerSheet = nullptr;
	IntVec2								m_laborerSheetDim = IntVec2(16, 16);
	SpriteSheet* 						m_testSheet = nullptr;
	IsoSpriteDefenition*				m_isoSprite = nullptr;

	float								m_quadSize = 1.f;

	Vec3								m_testDirection = Vec3(0.f, 0.f, 1.f);
};