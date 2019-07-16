#pragma once
#include "Engine/Renderer/Camera.hpp"

class CarCamera : public Camera
{
public:

	CarCamera();
	~CarCamera();

	void Update(const Vec3& carForward, float deltaTime);
	void SetFocalPoint(Vec3 const &pos);
	void SetZoom(float zoom); //Manipulates distance
	void SetAngleOffset(float angleOffset); // really is setting an angle offset
	void SetZoomDelta(float delta);

	void SetTiltValue(float tilt);
	void SetAngleValue(float angle);
	void SetHeightValue(float height);
	void SetDistanceValue(float distance);
	void SetLerpSpeed(float lerpSpeed);

	float GetAngleValue() const;
	float GetTiltValue() const;
	float GetHeightValue() const;
	float GetDistanceValue() const;
	float GetLerpSpeed() const;

private:
	Vec3			m_focalPoint = Vec3::ZERO;
	float			m_distance = 5.f;
	float			m_height = 2.f;

	float			m_currentZoom = 0.f;

	// configuration - mess with these numbers to get a view you like; 
	float			m_minDistance = 1.0f;
	float			m_maxDistance = 32.0f;

	float			m_defaultAngle = 90.0f;
	float			m_defaultTilt = -70.0f;
	float			m_lerpSpeed = 6.f;

	Vec2			m_tiltBounds = Vec2(10.f, 40.f);

	Vec3			m_camPosition = Vec3::ZERO;
	Vec3			m_targetPosition = Vec3::ZERO;

	Matrix44		m_modelMatrix = Matrix44::IDENTITY;

	//The actual tilt and angle for the camera
	float			m_tilt;
	float			m_angle;
};