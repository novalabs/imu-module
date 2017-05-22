/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/Middleware.hpp>
#include <core/mw/transport/RTCANTransport.hpp>

#include <core/snippets/CortexMxFaultHandlers.h>

#include <core/hw/EXT.hpp>
#include <core/hw/GPIO.hpp>
#include <core/hw/SPI.hpp>
#include <core/hw/SD.hpp>
#include <core/hw/IWDG.hpp>
#include <core/os/Thread.hpp>

#include <Module.hpp>

#include <core/L3GD20H_driver/L3GD20H.hpp>
#include <core/LSM303D_driver/LSM303D.hpp>

// LED
using LED_PAD = core::hw::Pad_<core::hw::GPIO_C, LED_PIN>;
static LED_PAD _led;

// ACC + MAG
using AM_PAD_CS = core::hw::Pad_<core::hw::GPIO_A, GPIOA_AM_CS>;
static core::hw::SPIDevice_<core::hw::SPI_1, AM_PAD_CS> AM_SPI_DEVICE;
static core::hw::EXTChannel_<core::hw::EXT_1, GPIOA_AM_INT1, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOA> ACC_EXT_CHANNEL;
static core::hw::EXTChannel_<core::hw::EXT_1, GPIOA_AM_INT2, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOA> MAG_EXT_CHANNEL;
static sensors::LSM303D     _am_device(AM_SPI_DEVICE, ACC_EXT_CHANNEL, MAG_EXT_CHANNEL);
static sensors::LSM303D_Acc _acc(_am_device);
static sensors::LSM303D_Mag _mag(_am_device);

// GYRO
using GYRO_PAD_CS = core::hw::Pad_<core::hw::GPIO_B, GPIOB_GYRO_CS>;
static core::hw::SPIDevice_<core::hw::SPI_1, GYRO_PAD_CS> GYRO_SPI_DEVICE;
static core::hw::EXTChannel_<core::hw::EXT_1, GPIOB_GYRO_INT2, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOB> GYRO_EXT_CHANNEL;
static sensors::L3GD20H      _gyro_device(GYRO_SPI_DEVICE, GYRO_EXT_CHANNEL);
static sensors::L3GD20H_Gyro _gyro(_gyro_device);

// MODULE DEVICES
sensors::L3GD20H_Gyro& Module::gyro = _gyro;
sensors::LSM303D_Acc&  Module::acc  = _acc;
sensors::LSM303D_Mag&  Module::mag  = _mag;

// DEVICE CONFIG
static const SPIConfig spi1cfg = {
    0, 0, 0, SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA, 0
};

static EXTConfig extcfg = {{
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL},
							   {EXT_CH_MODE_DISABLED, NULL}
						   }};

// SYSTEM STUFF
static core::os::Thread::Stack<1024> management_thread_stack;
static core::mw::RTCANTransport      rtcantra(&RTCAND1);

core::mw::Middleware
core::mw::Middleware::instance(
    ModuleConfiguration::MODULE_NAME
);


RTCANConfig rtcan_config = {
    1000000, 100, 60
};


Module::Module()
{}

bool
Module::initialize()
{
    FAULT_HANDLERS_ENABLE(true);

    static bool initialized = false;

    if (!initialized) {
        core::mw::CoreModule::initialize();

        core::mw::Middleware::instance.initialize(name(), management_thread_stack, management_thread_stack.size(), core::os::Thread::LOWEST);
        rtcantra.initialize(rtcan_config, canID());
        core::mw::Middleware::instance.start();

        spiStart(&SPID1, &spi1cfg);
        extStart(&EXTD1, &extcfg);

        _gyro_device.probe();
        _am_device.probe();

        gyro.init();
        acc.init();
        mag.init();

        initialized = true;
    }

    return initialized;
} // Board::initialize

// ----------------------------------------------------------------------------
// CoreModule STM32FlashConfigurationStorage
// ----------------------------------------------------------------------------
#include <core/snippets/CoreModuleSTM32FlashConfigurationStorage.hpp>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// CoreModule HW specific implementation
// ----------------------------------------------------------------------------
#include <core/snippets/CoreModuleHWSpecificImplementation.hpp>
// ----------------------------------------------------------------------------
