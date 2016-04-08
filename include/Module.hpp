#pragma once

#include <Configuration.hpp>
#include <Core/MW/CoreModule.hpp>
#include <Core/MW/CoreSensor.hpp>

namespace sensors {}

// Forward declarations
namespace sensors {
	class L3GD20H_Gyro;
	class LSM303D_Acc;
	class LSM303D_Mag;
}

class Module:
	public Core::MW::CoreModule
{
public:
// --- DEVICES ----------------------------------------------------------------
	static sensors::L3GD20H_Gyro& gyro;
	static sensors::LSM303D_Acc&  acc;
	static sensors::LSM303D_Mag&  mag;
// ----------------------------------------------------------------------------

	static bool
	initialize();


	Module();
	virtual ~Module() {}
};
