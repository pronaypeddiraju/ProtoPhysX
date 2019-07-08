#pragma once
#include "Engine/PhysXSystem/PhysXSystem.hpp"

class CarController
{
public:
	CarController();
	~CarController();

	void	SetupVehicle();
	void	SetDigitalControlMode(bool digitalControlEnabled);

	bool	IsDigitalInputEnabled() const;

	void	Update(float deltaTime);
	void	UpdateInputs(float deltaTime);
	void	VehiclePhysicsUpdate(float deltaTime);

	//Vehicle Getters
	PxVehicleDrive4W* GetVehicle() const;
	PxVehicleDrive4WRawInputData* GetVehicleInputData() const;

	//Vehicle Controls
	void	AccelerateForward(float analogAcc = 0.f);
	void	AccelerateReverse();
	void	Brake();

	void	TurnLeft();
	void	TurnRight();
	void	SetHandbrake();

	void	ReleaseAllControls();
private:


private:
	bool		m_digitalControlEnabled = false;
	bool		m_isVehicleInAir = false;

	PxVehicleDrive4W*					m_vehicle4W = nullptr;
	PxVehicleDrive4WRawInputData*		m_vehicleInputData = nullptr;

public:
	PxFixedSizeLookupTable<8>			m_SteerVsForwardSpeedTable;
	PxVehicleKeySmoothingData			m_keySmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	PxVehiclePadSmoothingData			m_padSmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};
};