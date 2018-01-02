/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/snippets/CortexMxFaultHandlers.h>

#include <core/mw/Middleware.hpp>
#include <core/mw/transport/RTCANTransport.hpp>

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

// DEVICES
static core::hw::EXTController_<core::hw::EXT_1> _ext;
static core::hw::SPIMaster_<core::hw::SPI_1>     _spi;

// ACC + MAG
static core::hw::EXTChannel_<core::hw::EXT_1, GPIOA_AM_INT1, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOA> _acc_interrupt_channel;
static core::hw::EXTChannel_<core::hw::EXT_1, GPIOA_AM_INT2, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOA> _mag_interrupt_channel;
static core::hw::SPIDevice_<core::hw::SPI_1, core::hw::Pad_<core::hw::GPIO_A, GPIOA_AM_CS> > _am_spi;
static sensors::LSM303D     _lsm303d(_am_spi, _acc_interrupt_channel, _mag_interrupt_channel);
static sensors::LSM303D_Acc _acc(_lsm303d);
static sensors::LSM303D_Mag _mag(_lsm303d);

// GYRO
static core::hw::EXTChannel_<core::hw::EXT_1, GPIOB_GYRO_INT2, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOB> _gyro_interrupt_channel;
static core::hw::SPIDevice_<core::hw::SPI_1, core::hw::Pad_<core::hw::GPIO_B, GPIOB_GYRO_CS> > _gyro_spi;
static sensors::L3GD20H      _l3gd20h(_gyro_spi, _gyro_interrupt_channel);
static sensors::L3GD20H_Gyro _gyro(_l3gd20h);

// MODULE DEVICES
sensors::L3GD20H_Gyro& Module::gyro = _gyro;
sensors::LSM303D_Acc&  Module::acc  = _acc;
sensors::LSM303D_Mag&  Module::mag  = _mag;

// DEVICE CONFIG
static const SPIConfig _spi_config = {
    0, 0, 0, SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA, 0
};

static EXTConfig _ext_config = {    {
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

RTCANConfig rtcan_config = {
    1000000, 100, 60
};


Module::Module()
{}

bool
Module::initialize()
{
#ifdef _DEBUG
    FAULT_HANDLERS_ENABLE(true);
#else
    FAULT_HANDLERS_ENABLE(false);
#endif

    static bool initialized = false;

    if (!initialized) {
        core::mw::CoreModule::initialize();

        core::mw::Middleware::instance().initialize(name(), management_thread_stack, management_thread_stack.size(), core::os::Thread::LOWEST);
        rtcantra.initialize(rtcan_config, canID());
        core::mw::Middleware::instance().start();

        _ext.start(_ext_config);
        _spi.start(_spi_config);

        _l3gd20h.probe();
        _lsm303d.probe();

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
