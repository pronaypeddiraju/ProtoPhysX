//------------------------------------------------------------------------------------------------------------------------------
#include "Game/CarController.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
//PhysX
#include "PxVehicleUtil.h"

//------------------------------------------------------------------------------------------------------------------------------
// GLOBAL DATA
//------------------------------------------------------------------------------------------------------------------------------
float gSteerVsForwardSpeedData[2 * 8] =
{
	//SteerAmount	to	Forward Speed
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
};

//------------------------------------------------------------------------------------------------------------------------------
CarController::CarController()
{
	SetupVehicle();
	m_vehicleInputData = new PxVehicleDrive4WRawInputData();

	//Setup the steer to speed table
	m_SteerVsForwardSpeedTable = PxFixedSizeLookupTable<8>(gSteerVsForwardSpeedData, 4);
}

//------------------------------------------------------------------------------------------------------------------------------
CarController::~CarController()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void CarController::SetupVehicle()
{
	m_vehicle4W = g_PxPhysXSystem->StartUpVehicleSDK();
}

//------------------------------------------------------------------------------------------------------------------------------
void CarController::SetDigitalControlMode(bool digitalControlEnabled)
{
	m_digitalControlEnabled = digitalControlEnabled;
}

//------------------------------------------------------------------------------------------------------------------------------
bool CarController::IsDigitalInputEnabled() const
{
	return m_digitalControlEnabled;
}

//------------------------------------------------------------------------------------------------------------------------------
void CarController::Update(float deltaTime)
{
	VehiclePhysicsUpdate(deltaTime);

	UpdateInputs(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------------
void CarController::UpdateInputs(float deltaTime)
{
	//Get Xbox controller data
	XboxController playerController = g_inputSystem->GetXboxController(0);
	AnalogJoyStick leftStick = playerController.GetLeftJoystick();
	AnalogJoyStick rightStick = playerController.GetRightJoystick();

	float rightTrigger = playerController.GetRightTrigger();

	if (rightTrigger > 0.1f)
	{
		AccelerateForward(rightTrigger);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void CarController::VehiclePhysicsUpdate(float deltaTime)
{
	VehicleSceneQueryData* vehicleSceneQueryData = g_PxPhysXSystem->GetVehicleSceneQueryData();
	PxScene* scene = g_PxPhysXSystem->GetPhysXScene();
	PxBatchQuery* batchQuery = g_PxPhysXSystem->GetPhysXBatchQuery();
	PxVehicleDrivableSurfaceToTireFrictionPairs* tireFrictionPairs = g_PxPhysXSystem->GetVehicleTireFrictionPairs();

	PxVehicleDrive4W* vehicle4W = GetVehicle();
	PxVehicleDrive4WRawInputData* vehicleInputData = GetVehicleInputData();

	if (vehicle4W == nullptr)
	{
		return;
	}

	//Update the control inputs for the vehicle.
	if (IsDigitalInputEnabled())
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(m_keySmoothingData, m_SteerVsForwardSpeedTable, *vehicleInputData, deltaTime, m_isVehicleInAir, *vehicle4W);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(m_padSmoothingData, m_SteerVsForwardSpeedTable, *vehicleInputData, deltaTime, m_isVehicleInAir, *vehicle4W);
	}

	//Raycasts.
	PxVehicleWheels* vehicles[1] = { vehicle4W };
	PxRaycastQueryResult* raycastResults = vehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = vehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(batchQuery, 1, vehicles, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = scene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, vehicle4W->mWheelsSimData.getNbWheels()} };
	PxVehicleUpdates(deltaTime, grav, *tireFrictionPairs, 1, vehicles, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	m_isVehicleInAir = vehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxVehicleDrive4W* CarController::GetVehicle() const
{
	return m_vehicle4W;
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxVehicleDrive4WRawInputData* CarController::GetVehicleInputData() const
{
	return m_vehicleInputData;
}

//------------------------------------------------------------------------------------------------------------------------------
void CarController::AccelerateForward(float analogAcc)
{
	if (m_digitalControlEnabled)
	{
		m_vehicleInputData->setDigitalAccel(true);
	}
	else
	{
		m_vehicleInputData->setAnalogAccel(analogAcc);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void CarController::AccelerateReverse()
{
	m_vehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

	if (m_digitalControlEnabled)
	{
		m_vehicleInputData->setDigitalAccel(true);
	}
	else
	{
		m_vehicleInputData->setAnalogAccel(1.f);
	}
}

