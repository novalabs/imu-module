#include <ModuleConfiguration.hpp>
#include <Module.hpp>

// MESSAGES
#include <core/common_msgs/Led.hpp>
#include <core/common_msgs/String64.hpp>
#include <core/sensor_msgs/RPY_f32.hpp>

// NODES
#include <core/sensor_publisher/Publisher.hpp>
#include <core/led/Publisher.hpp>
#include <core/led/Subscriber.hpp>
#include <core/madgwick/Madgwick.hpp>

// BOARD IMPL
#include <core/L3GD20H_driver/L3GD20H.hpp>
#include <core/LSM303D_driver/LSM303D.hpp>

// *** DO NOT MOVE ***
Module module;

// TYPES
using Vector3_i16_Publisher = core::sensor_publisher::Publisher<core::common_msgs::Vector3_i16>;

// NODES
Vector3_i16_Publisher gyro_publisher("gyro_publisher", module.gyro, core::os::Thread::PriorityEnum::NORMAL + 1);
Vector3_i16_Publisher acc_publisher("acc_publisher", module.acc, core::os::Thread::PriorityEnum::NORMAL + 1);
Vector3_i16_Publisher mag_publisher("mag_publisher", module.mag, core::os::Thread::PriorityEnum::NORMAL + 1);

core::led::Publisher     led_publisher("led_publisher", core::os::Thread::PriorityEnum::LOWEST);
core::led::Subscriber    led_subscriber("led_subscriber", core::os::Thread::PriorityEnum::LOWEST);
core::madgwick::Madgwick madgwick_filter("madgwick");

// MAIN
extern "C" {
   int
   main()
   {
      module.initialize();

      module.add(led_subscriber);
      module.add(gyro_publisher);
      module.add(acc_publisher);
      module.add(mag_publisher);
      module.add(madgwick_filter);

      // Led subscriber node
      core::led::SubscriberConfiguration led_subscriber_configuration;
      led_subscriber_configuration.topic = "led";
      led_subscriber.setConfiguration(led_subscriber_configuration);

      // Sensor nodes
      core::sensor_publisher::Configuration gyro_publisher_configuration;
      gyro_publisher_configuration.topic = "gyro";
      gyro_publisher.setConfiguration(gyro_publisher_configuration);

      core::sensor_publisher::Configuration acc_publisher_configuration;
      acc_publisher_configuration.topic = "acc";
      acc_publisher.setConfiguration(acc_publisher_configuration);

      core::sensor_publisher::Configuration mag_publisher_configuration;
      mag_publisher_configuration.topic = "mag";
      mag_publisher.setConfiguration(mag_publisher_configuration);

      // Madgwick filter node
      core::madgwick::MadgwickConfiguration madgwick_filter_configuration;
      madgwick_filter_configuration.topicGyro = gyro_publisher_configuration.topic;
      madgwick_filter_configuration.topicAcc  = acc_publisher_configuration.topic;
      madgwick_filter_configuration.topicMag  = mag_publisher_configuration.topic;
      madgwick_filter_configuration.topic     = "imu";
      madgwick_filter_configuration.frequency = 50.0f;

      madgwick_filter.setConfiguration(madgwick_filter_configuration);

      // Setup and run
      module.setup();
      module.run();

      // Is everything going well?
      for (;;) {
         if (!module.isOk()) {
            module.halt("This must not happen!");
         }

         module.keepAlive();

         core::os::Thread::sleep(core::os::Time::ms(500));
      }

      return core::os::Thread::OK;
   } // main
}
