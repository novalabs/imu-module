/* IMU Module template file
 *
 */

#include <ModuleConfiguration.hpp>
#include <Module.hpp>

// --- BOARD IMPL -------------------------------------------------------------
#include <core/L3GD20H_driver/L3GD20H.hpp>
#include <core/LSM303D_driver/LSM303D.hpp>

// --- MODULE -----------------------------------------------------------------
Module module;

// *** DO NOT MOVE THE CODE ABOVE THIS COMMENT *** //

// --- MESSAGES ---------------------------------------------------------------
#include <core/common_msgs/Led.hpp>
#include <core/common_msgs/String64.hpp>
#include <core/sensor_msgs/RPY_f32.hpp>

// --- NODES ------------------------------------------------------------------
#include <core/led/Subscriber.hpp>
#include <core/sensor_publisher/Publisher.hpp>
#include <core/madgwick/Madgwick.hpp>

// --- TYPES ------------------------------------------------------------------
using Vector3_i16_Publisher = core::sensor_publisher::Publisher<core::common_msgs::Vector3_i16>;

// --- CONFIGURATIONS ---------------------------------------------------------
core::led::SubscriberConfiguration       led_subscriber_configuration_default;
Vector3_i16_Publisher::ConfigurationType gyro_publisher_configuration_default;
Vector3_i16_Publisher::ConfigurationType acc_publisher_configuration_default;
Vector3_i16_Publisher::ConfigurationType mag_publisher_configuration_default;
core::madgwick::MadgwickConfiguration    madgwick_filter_configuration_default;

// --- NODES ------------------------------------------------------------------
core::led::Subscriber    led_subscriber("led_sub", core::os::Thread::PriorityEnum::LOWEST);
Vector3_i16_Publisher    gyro_publisher("gyro_publisher", module.gyro, core::os::Thread::PriorityEnum::NORMAL + 1);
Vector3_i16_Publisher    acc_publisher("acc_publisher", module.acc, core::os::Thread::PriorityEnum::NORMAL + 1);
Vector3_i16_Publisher    mag_publisher("mag_publisher", module.mag, core::os::Thread::PriorityEnum::NORMAL + 1);
core::madgwick::Madgwick madgwick_filter("madgwick");

// --- DEVICE CONFIGURATION ---------------------------------------------------

// --- MAIN -------------------------------------------------------------------
extern "C" {
    int
    main()
    {
        module.initialize();

        // Device configurations

        // Default configuration
        led_subscriber_configuration_default.topic      = "led";
        gyro_publisher_configuration_default.topic      = "gyro";
        acc_publisher_configuration_default.topic       = "acc";
        mag_publisher_configuration_default.topic       = "mag";
        madgwick_filter_configuration_default.topicGyro = gyro_publisher_configuration_default.topic;
        madgwick_filter_configuration_default.topicAcc  = acc_publisher_configuration_default.topic;
        madgwick_filter_configuration_default.topicMag  = mag_publisher_configuration_default.topic;
        madgwick_filter_configuration_default.topic     = "imu";
        madgwick_filter_configuration_default.frequency = 50.0f;

        // Add configurable objects to the configuration manager...
        module.configurations().add(led_subscriber, led_subscriber_configuration_default);
        module.configurations().add(gyro_publisher, gyro_publisher_configuration_default);
        module.configurations().add(acc_publisher, acc_publisher_configuration_default);
        module.configurations().add(mag_publisher, mag_publisher_configuration_default);
        module.configurations().add(madgwick_filter, madgwick_filter_configuration_default);

        // ... and load the configuration
        module.configurations().loadFrom(module.configurationStorage());

        // Add nodes to the node manager...
        module.nodes().add(led_subscriber);
        module.nodes().add(gyro_publisher);
        module.nodes().add(acc_publisher);
        module.nodes().add(mag_publisher);
        module.nodes().add(madgwick_filter);

        // ... and let's play!
        module.nodes().setup();
        module.nodes().run();

        // Is everything going well?
        for (;;) {
            if (!module.nodes().areOk()) {
                module.halt("This must not happen!");
            }

            core::os::Thread::sleep(core::os::Time::ms(500));

            // Remember to feed the (watch)dog!
            module.keepAlive();
        }

        return core::os::Thread::OK;
    } // main
}
