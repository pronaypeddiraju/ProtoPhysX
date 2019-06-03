#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
//------------------------------------------------------------------------------------------------------------------------------

class Entity
{
public:
	//Methods
	void Render();
	void RunFrame();
	void Update(float deltaTime);
	Entity(Game* currentGame);
	~Entity();

	//AccessMethods
	const Vec2& GetPosition() const { return m_position; }
	const Rgba& GetColor() const { return m_color; }
protected:
	//Variables
	Vec2 m_velocity = Vec2(0.f, 0.f);
	float m_rotationDegrees = 0.f;
	float m_uniformScale;
	float m_angularVelocity;
	Vec2 m_direction = Vec2(0.f,0.f);
	Rgba m_color;

	Game* m_game = nullptr;

	bool m_isAlive;
	bool m_isOffScreen;

	//Debug Quantities
	Vertex_PCU m_DebugVertexCosmetic[48];
	Vertex_PCU m_DebugVertexPhysics[48];
	unsigned int m_NUM_DEBUG_VERTICES;
public:
	//variables
	bool m_isGarbage;
	float m_physicsRadius;
	float m_cosmeticRadius;
	Vec2 m_position = Vec2(0.f, 0.f);

private:
	//Methods
};

