#include <iostream>
#include "RhAL.hpp"
#include <thread>
#include <stdio.h>


namespace RhAL {

void testDynaban() {
	StandardManager manager;

	//Import configuration in file
	manager.readConfig("../Tests/testDynaban.json");


	std::cout << manager.saveJSON().dump(4) << std::endl;
	int nbDevices = 0;
	if (manager.checkDevices()) {
		std::cout << "All the devices are fine :)" << std::endl;
	} else {
		std::cout << "At least a device is not present" << std::endl;
		for (const auto& it : manager.devContainer<RhAL::Device>()) {
			RhAL::Device * dev = it.second;
			if (!dev->isPresent()) {
				std::cout << it.first << " is not present !" << std::endl;
			}
		}
		return;
	}

	//Initialiaze the Manager
	manager.setScheduleMode(true);

	manager.exitEmergencyState();
	manager.flush();
	for (const auto& it : manager.devContainer<RhAL::Dynaban64>()) {
		RhAL::Dynaban64 * dev = it.second;
		dev->goalPosition() = 0;
	}
	manager.flush();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	float r = (float)360/4096.0;

	// Sinus in 4 poly
	float coefs1[5] = {169.9973127875532f*r, 1.2118904739507608f*r, -859.49525560910968f*r, 109.93882674890278f*r, 489.17556618589202f*r};
	float coefs2[5] = {0.0f, -532.49287882689202f*r, -29.078490997017791f*r, 1058.1470413492527f*r, -459.36643296722087f*r};
	float coefs3[5] = {-169.99731278755326f*r, -1.2118904739506096f*r, 859.49525560910888f*r, -109.93882674889758f*r, -489.17556618590021f*r};
	float coefs4[5] = {0.0f, 532.49287882689293f*r, 29.07849099701108f*r, -1058.1470413492355f*r, 459.3664329672057f*r};
	float coefs[4][5];
	memcpy(coefs[0], coefs1, 5*sizeof(float));
	memcpy(coefs[1], coefs2, 5*sizeof(float));
	memcpy(coefs[2], coefs3, 5*sizeof(float));
	memcpy(coefs[3], coefs4, 5*sizeof(float));

	r = 1.0/5000.0;
	float torques1[5] = {169.9973127875532f*r, 1.2118904739507608f*r, -859.49525560910968f*r, 109.93882674890278f*r, 489.17556618589202f*r};
	float torques2[5] = {0.0f, -532.49287882689202f*r, -29.078490997017791f*r, 1058.1470413492527f*r, -459.36643296722087f*r};
	float torques3[5] = {-169.99731278755326f*r, -1.2118904739506096f*r, 859.49525560910888f*r, -109.93882674889758f*r, -489.17556618590021f*r};
	float torques4[5] = {0.0f, 532.49287882689293f*r, 29.07849099701108f*r, -1058.1470413492355f*r, 459.3664329672057f*r};
	float torques[4][5];
	memcpy(torques[0], torques1, 5*sizeof(float));
	memcpy(torques[1], torques2, 5*sizeof(float));
	memcpy(torques[2], torques3, 5*sizeof(float));
	memcpy(torques[3], torques4, 5*sizeof(float));

	for (const auto& it : manager.devContainer<RhAL::Dynaban64>()) {
		RhAL::Dynaban64 * dev = it.second;
		dev->prepareFirstTrajectory(coefs[3], 5, torques[3], 5, 0.5);
		nbDevices++;
	}
	manager.flush();

	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	std::cout << "Go " << std::endl;
	for (const auto& it : manager.devContainer<RhAL::Dynaban64>()) {
		RhAL::Dynaban64 * dev = it.second;
		dev->startFirstTrajectoryNow(3);
	}
	manager.flush();
	int indexs[6] = {0, 0, 0, 0, 0, 0};

	while(true) {
		manager.flush();
		int i = -1;
		for (const auto& it : manager.devContainer<RhAL::Dynaban64>()) {
			i++;
			RhAL::Dynaban64 * dev = it.second;
			if(dev->isReadyForNextTrajectory()) {
				dev->updateNextTrajectory(coefs[indexs[i]], 5, torques[indexs[i]], 5, 0.5000);
//				std::cout << "New traj for " << it.first << std::endl;
				indexs[i] = (indexs[i] + 1)%4;
				//can't even print an array without a loop for god's sake
				std::cout << "Indexs : [";
				int min = 4;
				int max = -1;
				for (int i = 6 - 1; i >= 0; i--) {
					if (indexs[i] > max) {
						max = indexs[i];
					}
					if (indexs[i] < min) {
						min = indexs[i];
					}
					if (i == 0) {
					    std::cout << indexs[i] << "] " << std::endl;
					} else {
						std::cout << indexs[i] << ", ";
					}
					if ((max - min > 1) && !(max == 3 && min == 0)) {
						std::cout << "De-Synchronisation detected !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
						manager.getStatistics().print();
						return;
					}
				}


			}
		}
	}


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

