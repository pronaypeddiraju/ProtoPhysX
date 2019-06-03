#include "Game/Entity.hpp"

void Entity::Render()
{
	//m_game->
}

void Entity::RunFrame()
{


}

void Entity::Update(float deltaTime)
{
	UNUSED (deltaTime);
	if(m_position.x >= 0.f && m_position.x <= 200.f && m_position.y >= 0.f && m_position.y <= 100.f)
		m_isOffScreen = false;
	else
	{
		m_isOffScreen = true;
	}
}


Entity::Entity(Game* currentGame)
	: m_game (currentGame)
{
	m_isGarbage = false;
	m_isAlive = true;
}

Entity::~Entity()
{
}