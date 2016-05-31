#include <Configuration.hpp>
#include <Module.hpp>

// MESSAGES
#include <common_msgs/Led.hpp>
#include <common_msgs/String64.hpp>
#include <sensor_msgs/RPY_f32.hpp>

// NODES
#include <sensors/Publisher.hpp>
#include <led/Publisher.hpp>
#include <led/Subscriber.hpp>
#include <madgwick/Madgwick.hpp>
//@@NODE_HEADERS@@//

// BOARD IMPL
#include <sensors/L3GD20H.hpp>
#include <sensors/LSM303D.hpp>

// *** DO NOT MOVE ***
Module module;

// TYPES
using Vector3_i16_Publisher = sensors::Publisher<common_msgs::Vector3_i16>;

// NODES
Vector3_i16_Publisher gyro_publisher("gyro_publisher", module.gyro, Core::MW::Thread::PriorityEnum::NORMAL + 1);
Vector3_i16_Publisher acc_publisher("acc_publisher", module.acc, Core::MW::Thread::PriorityEnum::NORMAL + 1);
Vector3_i16_Publisher mag_publisher("mag_publisher", module.mag);

led::Publisher led_publisher("led_publisher", Core::MW::Thread::PriorityEnum::LOWEST);
led::Subscriber led_subscriber("led_subscriber", Core::MW::Thread::PriorityEnum::LOWEST);
madgwick::Madgwick   madgwick_filter("madgwick");

// MAIN
extern "C" {
	int
	main()
	{
		module.initialize();

		// Led subscriber node
		led_subscriber.configuration.topic = "led";
		module.add(led_subscriber);

		// Sensor nodes
		gyro_publisher.configuration.topic = "gyro";
		acc_publisher.configuration.topic  = "acc";
		mag_publisher.configuration.topic  = "mag";
		module.add(gyro_publisher);
		module.add(acc_publisher);
		module.add(mag_publisher);

		// Madgwick filter node
		madgwick_filter.configuration.topicGyro = gyro_publisher.configuration.topic;
		madgwick_filter.configuration.topicAcc  = acc_publisher.configuration.topic;
		madgwick_filter.configuration.topicMag  = mag_publisher.configuration.topic;
		madgwick_filter.configuration.topic     = "imu";
		madgwick_filter.configuration.frequency = 50.0f;
		module.add(madgwick_filter);

		// Setup and run
		module.setup();
		module.run();

		// Is everything going well?
		for (;;) {
			if (!module.isOk()) {
				module.halt("This must not happen!");
			}

			Core::MW::Thread::sleep(Core::MW::Time::ms(500));
		}

		return Core::MW::Thread::OK;
	}
}
