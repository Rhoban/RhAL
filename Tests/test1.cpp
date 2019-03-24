#include <iostream>
#include "RhAL.hpp"
#include <thread>

namespace RhAL
{
void printDevice(const RhAL::Device& dev)
{
  std::cout << "Dev: id:" << dev.id() << " name:" << dev.name() << std::endl;
  std::cout << "    RegistersBool:" << std::endl;
  for (const auto& it : dev.registersList().containerBool())
  {
    std::cout << "    --" << it.first << std::endl;
  }
  std::cout << "    RegistersInt:" << std::endl;
  for (const auto& it : dev.registersList().containerInt())
  {
    std::cout << "    --" << it.first << std::endl;
  }
  std::cout << "    RegistersFloat:" << std::endl;
  for (const auto& it : dev.registersList().containerFloat())
  {
    std::cout << "    --" << it.first << std::endl;
  }
  std::cout << "    ParametersBool:" << std::endl;
  for (const auto& it : dev.parametersList().containerBool())
  {
    std::cout << "    --" << it.first << std::endl;
  }
  std::cout << "    ParametersNumber:" << std::endl;
  for (const auto& it : dev.parametersList().containerNumber())
  {
    std::cout << "    --" << it.first << std::endl;
  }
  std::cout << "    ParametersStr:" << std::endl;
  for (const auto& it : dev.parametersList().containerStr())
  {
    std::cout << "    --" << it.first << std::endl;
  }
}

void testRX64()
{
  StandardManager manager;

  manager.setProtocolConfig("/dev/ttyUSB0", 1000000, "DynamixelV1");

  manager.scan();

  // Export configuration in file
  manager.writeConfig("/tmp/rhal.json");

  // Import configuration in file
  manager.readConfig("/tmp/rhal.json");

  std::cout << manager.saveJSON().dump(4) << std::endl;

  // Set Manager scheduling config mode
  //(default is true)
  manager.setScheduleMode(false);

  RhAL::RX64& dev = manager.dev<RhAL::RX64>(38);

  dev.enableTorque();

  float limit1 = dev.angleLimitCW();
  float limit2 = dev.angleLimitCCW();
  std::cout << "Angle limits = " << limit1 << ", " << limit2 << std::endl;

  float t = 0.0;
  int delay = 20;
  int slope1 = 1;
  int slope2 = 1;
  while (true)
  {
    std::cout << "Setting slope to " << slope1 << std::endl;
    dev.complianceSlopeCW() = slope1;
    dev.complianceSlopeCCW() = slope2;
    slope1 = dev.complianceSlopeCW();
    slope2 = dev.complianceSlopeCCW();
    std::cout << "Slopes read = " << slope1 << ", " << slope2 << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    while (true)
    {
      //				std::cout << "val " << 2*M_PI*2*t << std::endl;
      dev.goalPosition() = (40 * sin(2 * M_PI * 0.25 * t));
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
      t = t + delay / 1000.0;
      if (t >= 5.0)
      {
        t = 0;
        break;
      }
    }
    slope1++;
    slope2++;
    if (slope1 >= 8)
    {
      slope1 = 1;
      slope2 = 1;
    }
  }
}

void testMX64()
{
  StandardManager manager;

  manager.setProtocolConfig("/dev/ttyUSB0", 1000000, "DynamixelV1");
  //    manager.setProtocolConfig(
  //        "", 1000000, "FakeProtocol");

  // Scan the bus
  //(no response with FakeProtocol)
  manager.scan();

  // Export configuration in file
  manager.writeConfig("/tmp/rhal.json");

  // Import configuration in file
  manager.readConfig("/tmp/rhal.json");

  std::cout << manager.saveJSON().dump(4) << std::endl;

  //    manager.devAdd<RhAL::MX64>(1, "devTest1");

  // Set Manager scheduling config mode
  //(default is true)
  manager.setScheduleMode(false);

  RhAL::MX64& dev = manager.dev<RhAL::MX64>(1);

  //    std::cout << "enableTorque = " << dev.getTorqueEnable() << std::endl;

  dev.enableTorque();

  float limit1;
  float limit2;
  //    limits[0] = 0.0;
  //    limits[1] = 0.0;
  //    dev.setAngleLimits(limits);

  //    dev.setWheelMode();
  limit1 = dev.angleLimitCW();
  limit2 = dev.angleLimitCCW();
  std::cout << "Angle limits = " << limit1 << ", " << limit2 << std::endl;

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
  while (true)
  {
    pos = pos + 20 * direction;
    std::cout << "setting pos " << pos << std::endl;
    dev.goalPosition() = pos;
    std::cout << "Position read = " << dev.goalPosition() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    if (pos >= 720)
    {
      direction = -1 * direction;
    }
  }
  int i = 0;
  while (true)
  {
    pos = -20;
    std::cout << "setting pos " << pos << std::endl;
    dev.goalPosition() = pos;
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    pos = 0;
    std::cout << "setting pos " << pos << std::endl;
    dev.goalPosition() = pos;
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    pos = 20;
    std::cout << "setting pos " << pos << std::endl;
    dev.goalPosition() = pos;
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    i++;
    std::cout << "temp = " << dev.temperature() << std::endl;
    std::cout << "voltage = " << dev.voltage() << std::endl;
  }
  return;
}

void testMX64AndDynaban()
{
  StandardManager manager;

  manager.setProtocolConfig("/dev/ttyUSB0", 1000000, "DynamixelV1");

  // Scan the bus
  //(no response with FakeProtocol)
  manager.scan();

  // Export configuration in file
  manager.writeConfig("/tmp/rhal.json");

  // Import configuration in file
  manager.readConfig("/tmp/rhal.json");

  std::cout << manager.saveJSON().dump(4) << std::endl;

  // Set Manager scheduling config mode
  manager.setScheduleMode(false);
  while (true)
  {
    // Iterate over Manager Devices with types
    for (const auto& it : manager.devContainer<RhAL::Device>())
    {
      RhAL::Device* dev = it.second;

      if (manager.devTypeName(it.second->name()) == "MX64")
      {
        // Unchecked cast instead of dynamic_cast because the check was hand made :)
        RhAL::MX* devMx = (RhAL::MX*)dev;
        std::cout << "Position = " << devMx->position() << std::endl;
        devMx->goalPosition() = 180;
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

void testImuAndPins()
{
  StandardManager manager;

  manager.setProtocolConfig("/dev/ttyACM0", 1000000, "DynamixelV1");

  // Scan the bus
  //(no response with FakeProtocol)
  manager.scan();

  // Export configuration in file
  manager.writeConfig("/tmp/rhal.json");

  // Import configuration in file
  manager.readConfig("/tmp/rhal.json");

  std::cout << manager.saveJSON().dump(4) << std::endl;

  // Set Manager scheduling config mode
  manager.setScheduleMode(false);

  while (true)
  {
    std::cout << "-_-'" << std::endl;
    // Iterate over Manager Devices with types
    for (const auto& it : manager.devContainer<RhAL::Device>())
    {
      RhAL::Device* dev = it.second;

      if (manager.devTypeName(it.second->name()) == "IMU")
      {
        RhAL::IMU* devImu = (RhAL::IMU*)dev;

        std::cout << "yaw = " << devImu->yaw() << std::endl;
        std::cout << "pitch = " << devImu->pitch() << std::endl;
        std::cout << "roll = " << devImu->roll() << std::endl;
        std::cout << "accX = " << devImu->accX() << std::endl;
        std::cout << "accY = " << devImu->accY() << std::endl;
        std::cout << "accZ = " << devImu->accZ() << std::endl;
        std::cout << "gyroX = " << devImu->gyroX() << std::endl;
        std::cout << "gyroY = " << devImu->gyroY() << std::endl;
        std::cout << "gyroZ = " << devImu->gyroZ() << std::endl;
        std::cout << "gyroYaw = " << devImu->gyroYaw() << std::endl;
        std::cout << "magnX = " << devImu->magnX() << std::endl;
        std::cout << "magnY = " << devImu->magnY() << std::endl;
        std::cout << "magnZ = " << devImu->magnZ() << std::endl;
        std::cout << "magnAzimuth = " << devImu->magnAzimuth() << std::endl;
      }
      else if (manager.devTypeName(it.second->name()) == "Pins")
      {
        RhAL::Pins* devPins = (RhAL::Pins*)dev;
        bool buttons[7];
        devPins->getPins(buttons);
        std::cout << "Buttons = " << buttons[0] << ", " << buttons[1] << ", " << buttons[2] << ", " << buttons[3]
                  << ", " << buttons[4] << ", " << buttons[5] << ", " << buttons[6] << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return;
}

void testCM510()
{
  StandardManager manager;

  manager.setProtocolConfig("/dev/ttyS0", 1000000, "DynamixelV1");

  // Scan the bus
  //(no response with FakeProtocol)
  manager.scan();

  // Export configuration in file
  manager.writeConfig("/tmp/rhal.json");

  // Import configuration in file
  manager.readConfig("/tmp/rhal.json");

  std::cout << manager.saveJSON().dump(4) << std::endl;

  return;
}

void testDynaban()
{
  StandardManager manager;

  manager.setProtocolConfig("/dev/ttyUSB0", 1000000, "DynamixelV1");

  // Scan the bus
  //(no response with FakeProtocol)
  manager.scan();

  // Export configuration in file
  manager.writeConfig("/tmp/rhal.json");

  // Import configuration in file
  manager.readConfig("/tmp/rhal.json");

  std::cout << manager.saveJSON().dump(4) << std::endl;

  RhAL::Dynaban64& dev = manager.dev<RhAL::Dynaban64>(1);
  //	    RhAL::Dynaban64 * dev = dynamic_cast<RhAL::Dynaban64*>(&devFake);
  // Set Manager scheduling config mode
  manager.setScheduleMode(false);

  std::cout << "zero = " << dev.getZero() << std::endl;
  std::cout << "inverted = " << dev.getInverted() << std::endl;

  std::cout << "Position = " << dev.getPosition() << std::endl;

  manager.exitEmergencyState();
  float r = (float)2 * M_PI / 4096.0;

  // Sinus in 1 poly :
  //		float coefs[5] = {-29.674*r, 984.059*r, -1405.654*r, 438.301*r, 13.613*r};
  //		dev.prepareFirstTrajectory(coefs, 5, nullptr, 0, 2.0);
  //		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  //		std::cout << "Go " << std::endl;
  //		dev.startFirstTrajectoryNow(3);
  //		while(true) {
  //			while(!dev.isReadyForNextTrajectory());
  //			std::cout << "New poly " << std::endl;
  //			dev.updateNextTrajectory(coefs, 5, nullptr, 0, 2.0);
  //		}

  // Sinus in 4 poly
  float coefs1[5] = { 169.9973127875532f * r, 1.2118904739507608f * r, -859.49525560910968f * r,
                      109.93882674890278f * r, 489.17556618589202f * r };
  float coefs2[5] = { 0.0f, -532.49287882689202f * r, -29.078490997017791f * r, 1058.1470413492527f * r,
                      -459.36643296722087f * r };
  float coefs3[5] = { -169.99731278755326f * r, -1.2118904739506096f * r, 859.49525560910888f * r,
                      -109.93882674889758f * r, -489.17556618590021f * r };
  float coefs4[5] = { 0.0f, 532.49287882689293f * r, 29.07849099701108f * r, -1058.1470413492355f * r,
                      459.3664329672057f * r };

  dev.prepareFirstTrajectory(coefs4, 5, nullptr, 0, 0.5);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "Go " << std::endl;
  dev.startFirstTrajectoryNow(3);
  while (true)
  {
    while (!dev.isReadyForNextTrajectory())
      ;
    std::cout << "New poly 1" << std::endl;
    dev.updateNextTrajectory(coefs1, 5, nullptr, 0, 0.5);

    while (!dev.isReadyForNextTrajectory())
      ;
    std::cout << "New poly 2" << std::endl;
    dev.updateNextTrajectory(coefs2, 5, nullptr, 0, 0.5);

    while (!dev.isReadyForNextTrajectory())
      ;
    std::cout << "New poly 3" << std::endl;
    dev.updateNextTrajectory(coefs3, 5, nullptr, 0, 0.5);

    while (!dev.isReadyForNextTrajectory())
      ;
    std::cout << "New poly 4" << std::endl;
    dev.updateNextTrajectory(coefs4, 5, nullptr, 0, 0.5);
  }

  while (true)
    ;

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

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "Testing the frozen ram mode." << std::endl;
  dev.setTorqueEnable(false);
  dev.setGoalPosition(0);
  std::cout << "Setting frozen ram to ON " << std::endl;
  dev.setFrozenRamOn(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "Setting torque enable to true, the motor should NOT move right now !" << std::endl;
  dev.setTorqueEnable(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "Setting use values now. The motor SHOULD move." << std::endl;
  dev.setUseValuesNow(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "Setting torque enable to false and an other position order, the motor should NOT move" << std::endl;
  dev.setTorqueEnable(false);
  dev.setGoalPosition(90);
  std::cout << "Setting use values now. The motor should NOT move." << std::endl;
  dev.setUseValuesNow(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "Setting torque enable to true and use values now, the motor SHOULD move" << std::endl;
  dev.setTorqueEnable(true);
  dev.setGoalPosition(170);
  dev.setUseValuesNow(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  dev.setFrozenRamOn(false);
  dev.setUseValuesNow(true);
  std::cout << "***End of frozen mode test " << std::endl;

  std::cout << "Emergency stop ! " << std::endl;
  manager.emergencyStop();

  return;
}

void test()
{
  StandardManager manager;

  manager.setProtocolConfig("/dev/ttyUSB0", 1000000, "DynamixelV1");

  // Scan the bus
  //(no response with FakeProtocol)
  manager.scan();

  // Export configuration in file
  manager.writeConfig("/tmp/rhal.json");

  // Import configuration in file
  manager.readConfig("/tmp/rhal.json");

  std::cout << manager.saveJSON().dump(4) << std::endl;

  // Set Manager scheduling config mode
  manager.setScheduleMode(false);

  for (const auto& it : manager.devContainer<RhAL::Device>())
  {
    RhAL::Device* dev = it.second;
    if (manager.devTypeName(it.second->name()) == "MX64")
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      RhAL::MX* devMx = (RhAL::MX*)dev;
      std::cout << "srl = " << devMx->statusReturnLevel() << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

      devMx->statusReturnLevel() = 1;
    }
  }

  bool ledValue = true;
  while (true)
  {
    // Iterate over Manager Devices with types
    for (const auto& it : manager.devContainer<RhAL::Device>())
    {
      RhAL::Device* dev = it.second;

      if (manager.devTypeName(it.second->name()) == "MX64")
      {
        //					//Unchecked cast instead of dynamic_cast because the check was hand made :)
        RhAL::MX* devMx = (RhAL::MX*)dev;
        auto ledRegister = devMx->registersList().containerBool().at("led");
        auto value = ledRegister->readValue();
        // bool led = value.value;

        std::cout << "led = " << value.value << ", error = " << value.isError << std::endl;
        //					led = !led;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        ledValue = !ledValue;
        devMx->led() = ledValue;
      }
      //			RhAL::MX * devMx = dynamic_cast<RhAL::MX*>(dev);
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  manager.getStatistics().print();
  std::cout << "Emergency stop ! " << std::endl;
  manager.emergencyStop();

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  std::cout << "Release stop ! " << std::endl;
  manager.exitEmergencyState();

  return;
}

void testMowgly()
{
  StandardManager manager;

  manager.setProtocolConfig("/dev/ttyACM0", 1000000, "DynamixelV1");

  // Scan the bus
  //(no response with FakeProtocol)
  //	    manager.scan();

  // Export configuration in file
  //	    manager.writeConfig("./MowglyAuto.json");

  // Import configuration in file
  manager.readConfig("./Mowgly.json");

  if (manager.checkDevices())
  {
    std::cout << "All the devices are fine :)" << std::endl;
  }
  else
  {
    std::cout << "At least a device is not present" << std::endl;
    for (const auto& it : manager.devContainer<RhAL::Device>())
    {
      RhAL::Device* dev = it.second;
      if (!dev->isPresent())
      {
        std::cout << it.first << " is not present !" << std::endl;
      }
    }
    return;
  }

  //	    std::cout << manager.saveJSON().dump(4) << std::endl;

  // Set Manager scheduling config mode
  manager.setScheduleMode(false);

  while (true)
  {
    std::cout << "Dynaban : " << std::endl;

    for (const auto& it : manager.devContainer<RhAL::MX>())
    {
      RhAL::MX* devGen = it.second;
      if (manager.devTypeName(it.second->name()) == "Dynaban64")
      {
        RhAL::Dynaban64* dev = (RhAL::Dynaban64*)devGen;

        std::cout << dev->name() << " : " << dev->position() << std::endl;
        dev->enableTorque();
        dev->goalPosition(0);
      }
    }

    std::cout << "RX : " << std::endl;
    for (const auto& it : manager.devContainer<RhAL::RX>())
    {
      RhAL::RX* devGen = it.second;
      std::cout << devGen->name() << " : " << devGen->position() << std::endl;
      devGen->enableTorque();
      devGen->goalPosition() = 0;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "*******************************************************************" << std::endl;
  }

  manager.getStatistics().print();
  std::cout << "Emergency stop ! " << std::endl;
  manager.emergencyStop();

  return;
}

}  // namespace RhAL

/**
 * Manager Devices manipulation example
 */
int main()
{
  //	RhAL::test();

  //	RhAL::testDynaban();
  RhAL::testMowgly();
  return 0;
}
