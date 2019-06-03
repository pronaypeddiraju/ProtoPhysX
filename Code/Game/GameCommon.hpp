#pragma once
#include "Engine/Commons/EngineCommon.hpp"

constexpr int NUM_STARTING_ASTEROIDS = 6;
constexpr int MAX_ASTEROIDS = 10;
constexpr int MAX_BULLETS = 40;
constexpr int MAX_BEETLES = 5;
constexpr int MAX_WASPS = 5;
constexpr int MAX_DEBRIS = 20;
constexpr float WORLD_WIDTH = 200.f;
constexpr float WORLD_HEIGHT = 100.f;
constexpr float WORLD_CENTER_X = WORLD_WIDTH / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_HEIGHT / 2.f;
constexpr float SCREEN_ASPECT = 16.f/9.f;
constexpr float ASTEROID_SPEED = 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS = 2.0f;
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;
constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;
constexpr float BEETLE_SPEED = 10.f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.0f;
constexpr float BEETLE_PHYSICS_RADIUS = 1.6f;

constexpr float WASP_SPEED = 20.f;
constexpr float WASP_COSMETIC_RADIUS = 3.0f;
constexpr float WASP_PHYSICS_RADIUS = 1.0f;

constexpr float DEBRIS_SPEED = 7.0f;
constexpr float DEBRIS_LIFETIME_SECONDS = 2.0f;

constexpr float CAMERA_SHAKE_REDUCTION_PER_SECOND = 1.f;
constexpr float MAX_SHAKE = 2.0f;

constexpr float DEVCONSOLE_LINE_HEIGHT = 2.0f;

constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area

class RenderContext;
class InputSystem;
class AudioSystem;

extern RenderContext* g_renderContext;
extern InputSystem* g_inputSystem;
extern AudioSystem* g_audio;