/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#include <Core/MW/Middleware.hpp>

#include "ch.h"
#include "hal.h"

#include <Core/HW/SPI.hpp>
#include <Core/HW/EXT.hpp>
#include <Core/MW/Thread.hpp>

#include <Module.hpp>

#include <L3GD20H_driver/L3GD20H.hpp>
#include <LSM303D_driver/LSM303D.hpp>

using AM_PAD_CS = Core::HW::Pad_<Core::HW::GPIO_A, GPIOA_AM_CS>;
static Core::HW::SPIDevice_<Core::HW::SPI_1, AM_PAD_CS> AM_SPI_DEVICE;
static Core::HW::EXTChannel_<Core::HW::EXT_1, GPIOA_AM_INT1, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOA> ACC_EXT_CHANNEL;
static Core::HW::EXTChannel_<Core::HW::EXT_1, GPIOA_AM_INT2, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOA> MAG_EXT_CHANNEL;

using GYRO_PAD_CS = Core::HW::Pad_<Core::HW::GPIO_B, GPIOB_GYRO_CS>;
static Core::HW::SPIDevice_<Core::HW::SPI_1, GYRO_PAD_CS> GYRO_SPI_DEVICE;
static Core::HW::EXTChannel_<Core::HW::EXT_1, GPIOB_GYRO_INT2, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOB> GYRO_EXT_CHANNEL;

using LED_PAD = Core::HW::Pad_<Core::HW::GPIO_C, LED_PIN>;

static LED_PAD _led;

static sensors::L3GD20H      _gyro_device(GYRO_SPI_DEVICE, GYRO_EXT_CHANNEL);
static sensors::L3GD20H_Gyro _gyro(_gyro_device);
static sensors::LSM303D      _am_device(AM_SPI_DEVICE, ACC_EXT_CHANNEL, MAG_EXT_CHANNEL);
static sensors::LSM303D_Acc  _acc(_am_device);
static sensors::LSM303D_Mag  _mag(_am_device);

sensors::L3GD20H_Gyro& Module::gyro = _gyro;
sensors::LSM303D_Acc&  Module::acc  = _acc;
sensors::LSM303D_Mag&  Module::mag  = _mag;

static const SPIConfig spi1cfg = {
   0, 0, 0, SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA, 0
};

static THD_WORKING_AREA(wa_info, 1024);
static Core::MW::RTCANTransport rtcantra(RTCAND1);

RTCANConfig rtcan_config = {
   1000000, 100, 60
};

#ifndef CORE_MODULE_NAME
#define CORE_MODULE_NAME "IMU"
#endif

Core::MW::Middleware Core::MW::Middleware::instance(CORE_MODULE_NAME, "BOOT_" CORE_MODULE_NAME);

static EXTConfig extcfg = {   {
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

Module::Module()
{}

bool
Module::initialize()
{
//	CORE_ASSERT(Core::MW::Middleware::instance.is_stopped()); // TODO: capire perche non va...

   static bool initialized = false;

   if (!initialized) {
      halInit();
      chSysInit();

      Core::MW::Middleware::instance.initialize(wa_info, sizeof(wa_info), Core::MW::Thread::LOWEST);
      rtcantra.initialize(rtcan_config);
      Core::MW::Middleware::instance.start();


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

// Leftover from CoreBoard (where LED_PAD cannot be defined
void
Core::MW::CoreModule::Led::toggle()
{
   _led.toggle();
}

void
Core::MW::CoreModule::Led::write(
   unsigned on
)
{
   _led.write(on);
}
