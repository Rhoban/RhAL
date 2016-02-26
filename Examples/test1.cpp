#include <iostream>
#include "RhAL.hpp"
#include <thread>


namespace RhAL {

void printDevice(const RhAL::Device& dev)
{
    std::cout 
        << "Dev: id:" << dev.id() 
        << " name:" << dev.name()
        << std::endl;
    std::cout << "    RegistersBool:" << std::endl;
    for (const auto& it : dev.registersList().containerBool()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    RegistersInt:" << std::endl;
    for (const auto& it : dev.registersList().containerInt()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    RegistersFloat:" << std::endl;
    for (const auto& it : dev.registersList().containerFloat()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    ParametersBool:" << std::endl;
    for (const auto& it : dev.parametersList().containerBool()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    ParametersNumber:" << std::endl;
    for (const auto& it : dev.parametersList().containerNumber()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    ParametersStr:" << std::endl;
    for (const auto& it : dev.parametersList().containerStr()) {
        std::cout << "    --" << it.first << std::endl;
    }
}

void testRX64() {
	StandardManager manager;

	    manager.setProtocolConfig(
	        "/dev/ttyUSB0", 1000000, "DynamixelV1");

	    manager.scan();

	    //Export configuration in file
	    manager.writeConfig("/tmp/rhal.json");

	    //Import configuration in file
	    manager.readConfig("/tmp/rhal.json");


	    std::cout << manager.saveJSON().dump(4) << std::endl;


	    //Set Manager scheduling config mode
	    //(default is true)
	    manager.setScheduleMode(false);

	    RhAL::RX64& dev = manager.dev<RhAL::RX64>(38);

	    dev.enableTorque();

	    float limits[2];
		dev.getAngleLimits(limits);
		std::cout << "Angle limits = " << limits[0] << ", " << limits[1] << std::endl;

		float t = 0.0;
		int delay = 20;
		int slope = 1;
		while (true) {
	    	std::cout << "Setting slope to " << slope << std::endl;
	    	int slopes[2] = {slope, slope};
	    	dev.setComplianceSlopes(slopes);
	    	dev.getComplianceSlopes(slopes);
	    	std::cout << "Slopes read = " << slopes[0] << ", " << slopes[1] << std::endl;
	    	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			while (true) {
//				std::cout << "val " << 2*M_PI*2*t << std::endl;
				dev.setGoalPosition(40*sin(2*M_PI*0.25*t));
				std::this_thread::sleep_for(std::chrono::milliseconds(delay));
				t = t + delay/1000.0;
				if (t >= 5.0) {
					t = 0;
					break;
				}
			}
			slope++;
			if (slope >= 8) {
				slope = 1;
			}
		}
}

void testMX64() {
	StandardManager manager;

	    manager.setProtocolConfig(
	        "/dev/ttyUSB0", 1000000, "DynamixelV1");
	//    manager.setProtocolConfig(
	//        "", 1000000, "FakeProtocol");

	    //Scan the bus
	    //(no response with FakeProtocol)
	    manager.scan();


	    //Export configuration in file
	    manager.writeConfig("/tmp/rhal.json");

	    //Import configuration in file
	    manager.readConfig("/tmp/rhal.json");


	    std::cout << manager.saveJSON().dump(4) << std::endl;

	//    manager.devAdd<RhAL::MX64>(1, "devTest1");


	    //Set Manager scheduling config mode
	    //(default is true)
	    manager.setScheduleMode(false);

	    RhAL::MX64& dev = manager.dev<RhAL::MX64>(1);

	//    std::cout << "enableTorque = " << dev.getTorqueEnable() << std::endl;


	    dev.enableTorque();

	    float limits[2];
	//    limits[0] = 0.0;
	//    limits[1] = 0.0;
	//    dev.setAngleLimits(limits);

	//    dev.setWheelMode();
		dev.getAngleLimits(limits);
		std::cout << "Angle limits = " << limits[0] << ", " << limits[1] << std::endl;


	//    std::cout << "Setting wheel mode !" << std::endl;
	//    dev.setWheelMode();
	//    std::cout << "Done." << std::endl;
	//    dev.getAngleLimits(limits);
	//    std::cout << "Angle limits = " << limits[0] << ", " << limits[1] << std::endl;
	//
	//    std::cout << "Setting speed command" << std::endl;
	//    dev.setGoalSpeed(100);
	//    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	//    dev.setGoalSpeed(0);
	//
	//    std::cout << "Setting joint mode = " << std::endl;
	//    dev.setJointMode();
	//	dev.getAngleLimits(limits);
	//    std::cout << "Angle limits = " << limits[0] << ", " << limits[1] << std::endl;

		float pos = 0.0;
		int direction = 1;
		// Going from 0 to 720 then 720 to 0 degrees
		while (true) {
			pos = pos + 20 * direction;
			std::cout << "setting pos " << pos << std::endl;
			dev.setGoalPosition(pos);
			std::cout << "Position read = " << dev.getGoalPosition() << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(400));
			if (pos >= 720) {
				direction = -1 * direction;
			}
		}
	    int i = 0;
	    while (true) {
	    	pos = -20;
	    	std::cout << "setting pos " << pos << std::endl;
	    	dev.setGoalPosition(pos);
	        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

	        pos = 0;
			std::cout << "setting pos " << pos << std::endl;
			dev.setGoalPosition(pos);
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));

			pos = 20;
			std::cout << "setting pos " << pos << std::endl;
			dev.setGoalPosition(pos);
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));

	        i++;
	    	std::cout << "temp = " << dev.getTemperature() << std::endl;
	    	std::cout << "voltage = " << dev.getVoltage() << std::endl;
	    }
	    return;
}

void testMX64AndDynaban() {
	StandardManager manager;

	    manager.setProtocolConfig(
	        "/dev/ttyUSB0", 1000000, "DynamixelV1");

	    //Scan the bus
	    //(no response with FakeProtocol)
	    manager.scan();

	    //Export configuration in file
	    manager.writeConfig("/tmp/rhal.json");

	    //Import configuration in file
	    manager.readConfig("/tmp/rhal.json");


	    std::cout << manager.saveJSON().dump(4) << std::endl;


	    //Set Manager scheduling config mode
	    manager.setScheduleMode(false);
	    while(true)
	    {
			//Iterate over Manager Devices with types
			for (const auto& it : manager.devContainer<RhAL::Device>()) {
				RhAL::Device * dev = it.second;

				if (manager.devTypeName(it.second->name()) == "MX64") {
					//Unchecked cast instead of dynamic_cast because the check was hand made :)
					RhAL::MX * devMx = (RhAL::MX*) dev;
					std::cout << "Position = " << devMx->getPositionRad() << std::endl;
					devMx->setGoalPosition(180);
				}
	//			RhAL::MX * devMx = dynamic_cast<RhAL::MX*>(dev);

			}
	    }
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		std::cout << "Emergency stop ! " << std::endl;
		manager.emergencyStop();

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		std::cout << "Release stop ! " << std::endl;
		manager.exitEmergencyState();

	    return;
}

void testImuAndPins() {
	StandardManager manager;

	    manager.setProtocolConfig(
	        "/dev/ttyACM0", 1000000, "DynamixelV1");

	    //Scan the bus
	    //(no response with FakeProtocol)
	    manager.scan();

	    //Export configuration in file
	    manager.writeConfig("/tmp/rhal.json");

	    //Import configuration in file
	    manager.readConfig("/tmp/rhal.json");


	    std::cout << manager.saveJSON().dump(4) << std::endl;


	    //Set Manager scheduling config mode
	    manager.setScheduleMode(false);

	    while(true)
	    {
	    	std::cout << "-_-'" << std::endl;
			//Iterate over Manager Devices with types
			for (const auto& it : manager.devContainer<RhAL::Device>()) {
				RhAL::Device * dev = it.second;

				if (manager.devTypeName(it.second->name()) == "IMU") {
					RhAL::IMU* devImu = (RhAL::IMU*) dev;

					std::cout << "yaw = " << devImu->getYaw() << std::endl;
					std::cout << "pitch = " << devImu->getPitch() << std::endl;
					std::cout << "roll = " << devImu->getRoll() << std::endl;
					std::cout << "accX = " << devImu->getAccX() << std::endl;
					std::cout << "accY = " << devImu->getAccY() << std::endl;
					std::cout << "accZ = " << devImu->getAccZ() << std::endl;
					std::cout << "gyroX = " << devImu->getGyroX() << std::endl;
					std::cout << "gyroY = " << devImu->getGyroY() << std::endl;
					std::cout << "gyroZ = " << devImu->getGyroZ() << std::endl;
					std::cout << "gyroYaw = " << devImu->getGyroYaw() << std::endl;
					std::cout << "magnX = " << devImu->getMagnX() << std::endl;
					std::cout << "magnY = " << devImu->getMagnY() << std::endl;
					std::cout << "magnZ = " << devImu->getMagnZ() << std::endl;
					std::cout << "magnAzimuth = " << devImu->getMagnAzimuth() << std::endl;

				} else if (manager.devTypeName(it.second->name()) == "Pins") {
					RhAL::Pins* devPins = (RhAL::Pins*) dev;
					bool buttons[7];
					devPins->getPins(buttons);
					std::cout << "Buttons = " << buttons[0] << ", " << buttons[1] << ", " << buttons[2]
					<< ", " << buttons[3] << ", " << buttons[4] << ", " << buttons[5] << ", " << buttons[6] << std::endl;
				}

			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	    }

	    return;
}

void testCM510() {
	StandardManager manager;

	    manager.setProtocolConfig(
	        "/dev/ttyS0", 1000000, "DynamixelV1");

	    //Scan the bus
	    //(no response with FakeProtocol)
	    manager.scan();

	    //Export configuration in file
	    manager.writeConfig("/tmp/rhal.json");

	    //Import configuration in file
	    manager.readConfig("/tmp/rhal.json");


	    std::cout << manager.saveJSON().dump(4) << std::endl;

	    return;
}

void testDynaban() {
	StandardManager manager;

	    manager.setProtocolConfig(
	        "/dev/ttyUSB0", 1000000, "DynamixelV1");

	    //Scan the bus
	    //(no response with FakeProtocol)
	    manager.scan();

	    //Export configuration in file
	    manager.writeConfig("/tmp/rhal.json");

	    //Import configuration in file
	    manager.readConfig("/tmp/rhal.json");


	    std::cout << manager.saveJSON().dump(4) << std::endl;

	    RhAL::Dynaban64& dev = manager.dev<RhAL::Dynaban64>(1);
//	    RhAL::Dynaban64 * dev = dynamic_cast<RhAL::Dynaban64*>(&devFake);
	    //Set Manager scheduling config mode
	    manager.setScheduleMode(false);

	    // Allowing torque
	    manager.exitEmergencyState();
		std::cout << "Position = " << dev.getPosition() << std::endl;

		dev.setGoalPosition(179);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "Position = " << dev.getPosition() << std::endl;
		dev.setGoalPosition(90);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::cout << "Position = " << dev.getPosition() << std::endl;
		dev.setGoalPosition(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::cout << "Position = " << dev.getPosition() << std::endl;
		dev.setGoalPosition(-90);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::cout << "Position = " << dev.getPosition() << std::endl;
		dev.setGoalPosition(-180);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::cout << "Position = " << dev.getPosition() << std::endl;


		std::cout << "Emergency stop ! " << std::endl;
		manager.emergencyStop();


	    return;
}

}

/**
 * Manager Devices manipulation example
 */
int main() {
	RhAL::testDynaban();
	return 0;
}

