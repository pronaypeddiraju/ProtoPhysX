#include "Game/CarCamera.hpp"
//Engine Systems
#include "Engine/Math/MathUtils.hpp"
//Game Systems
#include "Game/GameCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
CarCamera::CarCamera()
	: Camera()
{
	m_tilt = m_defaultTilt;
	m_angle = m_defaultAngle;
	m_currentZoom = RangeMapFloat(m_distance, m_minDistance, m_maxDistance, 0.f, 1.f);
}

//------------------------------------------------------------------------------------------------------------------------------
CarCamera::~CarCamera()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void CarCamera::Update(const Vec3& carForward)
{
	Vec3 offset = carForward * m_distance * -1.f;
	offset += Vec3::UP * m_height;

	m_targetPosition = m_focalPoint + offset;
	//We will lerp this value to m_target position next
	m_camPosition = m_focalPoint + offset;

	//Give me a model matrix of something at offset looking at the focal point. This is now my model matrix
	m_modelMatrix = Matrix44::LookAt(m_camPosition, m_focalPoint, Vec3(0.f, 1.f, 0.f));
	
	//m_modelMatrix.SetKBasis(carForward);
	SetModelMatrix(m_modelMatrix);
}

//------------------------------------------------------------------------------------------------------------------------------
void CarCamera::SetFocalPoint(Vec3 const &pos)
{
	m_focalPoint = pos;
}

//------------------------------------------------------------------------------------------------------------------------------
void CarCamera::SetZoom(float zoom)
{
	m_currentZoom = zoom;
	m_distance = RangeMapFloat(m_currentZoom, 0.f, 1.f, m_minDistance, m_maxDistance);
}

//------------------------------------------------------------------------------------------------------------------------------
void CarCamera::SetAngleOffset(float angleOffset)
{
	m_angle += angleOffset;
}

//------------------------------------------------------------------------------------------------------------------------------
void CarCamera::SetZoomDelta(float delta)
{
	m_currentZoom += delta;
	m_distance = RangeMapFloat(m_currentZoom, MIN_ZOOM_STEPS, MAX_ZOOM_STEPS, m_minDistance, m_maxDistance);

	m_distance = Clamp(m_distance, m_minDistance, m_maxDistance);
}

//------------------------------------------------------------------------------------------------------------------------------
void CarCamera::SetTiltValue(float tilt)
{
	m_tilt = tilt;
}

//------------------------------------------------------------------------------------------------------------------------------
void CarCamera::SetAngleValue(float angle)
{
	m_angle = angle;
}

//------------------------------------------------------------------------------------------------------------------------------
void CarCamera::SetHeightValue(float height)
{
	m_height = height;
}

//------------------------------------------------------------------------------------------------------------------------------
void CarCamera::SetDistanceValue(float distance)
{
	m_distance = distance;
}

//------------------------------------------------------------------------------------------------------------------------------
float CarCamera::GetAngleValue() const
{
	return m_angle;
}

//------------------------------------------------------------------------------------------------------------------------------
float CarCamera::GetTiltValue() const
{
	return m_tilt;
}

//------------------------------------------------------------------------------------------------------------------------------
float CarCamera::GetHeightValue() const
{
	return m_height;
}

//------------------------------------------------------------------------------------------------------------------------------
float CarCamera::GetDistanceValue() const
{
	return m_distance;
}
