/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/os/Time.hpp>

#include <core/common_msgs/Vector3_i16.hpp>

namespace ModuleConfiguration {
// --- CONSTANTS --------------------------------------------------------------
#ifdef CORE_MODULE_NAME
static const char* MODULE_NAME __attribute__((unused)) = CORE_MODULE_NAME;
#else
static const char* MODULE_NAME __attribute__((unused)) = "IMU";
#endif


static const core::os::Time PUBLISHER_RETRY_DELAY = core::os::Time::ms(500);
static const core::os::Time SUBSCRIBER_SPIN_TIME  = core::os::Time::ms(2000);

static const std::size_t SUBSCRIBER_QUEUE_LENGTH = 5;

// --- TYPES ------------------------------------------------------------------
using L3GD20H_GYRO_DATATYPE = core::common_msgs::Vector3_i16;
using LSM303D_ACC_DATATYPE  = core::common_msgs::Vector3_i16;
using LSM303D_MAG_DATATYPE  = core::common_msgs::Vector3_i16;
}
