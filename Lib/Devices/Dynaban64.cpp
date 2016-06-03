#include "Devices/Dynaban64.hpp"

// All the positions have an offset of 180 degrees (also true for the MXs and the RXs). This is because dxl's zeros are not the same among different servomotors, so we unified them.
#define OFFSET 180

namespace RhAL {

void convEncode_PolyDuration(data_t* buffer, float value)
{
	if (value < 0.0) {
		value = 0.0;
	} else if (value > 5.6) {
		value = 5.6;
	}

	uint16_t result = std::lround(value*10000);
	write2BytesToBuffer(buffer, result);
}

float convDecode_PolyDuration(const data_t* buffer)
{
	uint16_t val = read2BytesFromBuffer(buffer);
	float result = (float)val/10000.0;

	return result;
}

void convEncode_speedDelay(data_t* buffer, float value)
{
	if (value < 0.0) {
		value = 0.0;
	} else if (value > 5.6) {
		value = 5.6;
	}

	uint16_t result = std::lround(value*1000);
	write2BytesToBuffer(buffer, result);
}

float convDecode_speedDelay(const data_t* buffer)
{
	uint16_t val = read2BytesFromBuffer(buffer);
	float result = (float)val/1000.0;

	return result;
}

void convEncode_positionTraj(data_t* buffer, float positionInDegrees) {
	float result = positionInDegrees * 4096/(360.0);
	writeFloatToBuffer(buffer, result);
}

float convDecode_positionTraj(const data_t* buffer) {
	float val = readFloatFromBuffer(buffer);
	float result = val*360.0/4096.0;

	return result;
}


Dynaban64::Dynaban64(const std::string& name, id_t id) :
    MX64(name, id),
	//_register("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=true, forceWrite=false, isSlow=false)
	_trajPoly1Size("trajPoly1Size", 0x4A, 1, convEncode_1Byte, convDecode_1Byte, 0),
	_traj1a0("traj1a0", 0x4B, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, (value + this->_zero.value + OFFSET)*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value - (this->_zero.value - OFFSET);
	            return value;
	        }, 0),
	_traj1a1("traj1a1", 0x4F, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_traj1a2("traj1a2", 0x53, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_traj1a3("traj1a3", 0x57, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_traj1a4("traj1a4", 0x5B, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torquePoly1Size("torquePoly1Size", 0x5F, 1, convEncode_1Byte, convDecode_1Byte, 0),
	_torque1a0("torque1a0", 0x60, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torque1a1("torque1a1", 0x64, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torque1a2("torque1a2", 0x68, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torque1a3("torque1a3", 0x6C, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torque1a4("torque1a4", 0x70, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_duration1("duration1", 0x74, 2, convEncode_PolyDuration, convDecode_PolyDuration, 0),
	_trajPoly2Size("trajPoly2Size", 0x76, 1, convEncode_1Byte, convDecode_1Byte, 0),
	_traj2a0("traj2a0", 0x77, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, (value + this->_zero.value + OFFSET)*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value - (this->_zero.value - OFFSET);
	            return value;
	        }, 0),
	_traj2a1("traj2a1", 0x7B, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_traj2a2("traj2a2", 0x7F, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_traj2a3("traj2a3", 0x83, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_traj2a4("traj2a4", 0x87, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torquePoly2Size("torquePoly2Size", 0x8B, 1, convEncode_1Byte, convDecode_1Byte, 0),
	_torque2a0("torque2a0", 0x8C, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torque2a1("torque2a1", 0x90, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torque2a2("torque2a2", 0x94, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torque2a3("torque2a3", 0x98, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_torque2a4("torque2a4", 0x9C, 4,
	        [this](data_t* data, float value) {
	            std::lock_guard<std::mutex> lock(this->_mutex);
	            int direction = 1;
	    		if (this->_inverted.value == true) {
	    			direction = -1;
	    		}
	    		convEncode_positionTraj(data, value*direction);
	        }, [this](const data_t* data) -> float {
	            std::lock_guard<std::mutex> lock(_mutex);
	            float value = convDecode_positionTraj(data);
	            if (this->_inverted.value == true) {
	                value = value * -1.0;
	            }
	            value = value;
	            return value;
	        }, 0),
	_duration2("duration2", 0xA0, 2, convEncode_PolyDuration, convDecode_PolyDuration, 0),

	_mode("mode", 0xA2, 1, convEncode_1Byte, convDecode_1Byte, 0),
	_copyNextBuffer("copyNextBuffer", 0xA3, 1, convEncode_1Byte, convDecode_1Byte, 1, false),
	_positionTrackerOn("positionTrackerOn", 0xA4, 1, convEncode_Bool, convDecode_Bool, 0),
	_debugOn("debugOn", 0xA5, 1, convEncode_Bool, convDecode_Bool, 0),
	_unused("unused", 0xA6, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
	_i0("i0", 0xA8, 4, convEncode_float, convDecode_float, 0),
	_r("r", 0xAC, 4, convEncode_float, convDecode_float, 0),
	_ke("ke", 0xB0, 4, convEncode_float, convDecode_float, 0),
	_kvis("kvis", 0xB4, 4, convEncode_float, convDecode_float, 0),
	_kstat("kstat", 0xB8, 4, convEncode_float, convDecode_float, 0),
	_kcoul("kcoul", 0xBC, 4, convEncode_float, convDecode_float, 0),
	_linearTransition("linearTransition", 0xC0, 4, convEncode_float, convDecode_float, 0),
	_speedCalculationDelay("speedCalculationDelay", 0xC4, 2, convEncode_speedDelay, convDecode_speedDelay, 0),
	_outputTorque("ouputTorque", 0xC6, 4, convDecode_float, 1, false), // Read only
	_electricalTorque("electricalTorque", 0xCA, 4, convDecode_float, 0), // Read Only
	_frozenRamOn("frozenRamOn", 0xCE, 1, convEncode_Bool, convDecode_Bool, 0),
	_useValuesNow("useValuesNow", 0xCF, 1, convEncode_Bool, convDecode_Bool, 0),
	_torqueKp("torqueKp", 0xD0, 1, convEncode_2Bytes, convDecode_2Bytes, 0),
	_goalTorque("goalTorque", 0xD2, 4, convEncode_float, convDecode_float, 0),
	_predictiveCommandPeriod("predictiveCommandPeriod", 0xD6, 1, convEncode_float, convDecode_1Byte, 0)
{
    _duration1.setMinValue(0.0);
    _duration1.setMaxValue(6.5);
    _duration1.setStepValue(0.0001);

    _duration2.setMinValue(0.0);
    _duration2.setMaxValue(6.5);
    _duration2.setStepValue(0.0001);

    _speedCalculationDelay.setMinValue(0);
    _speedCalculationDelay.setMaxValue(6.5);
    _speedCalculationDelay.setStepValue(0.001);

}

void Dynaban64::onInit()
{
	MX::onInit();
	Device::registersList().add(&_trajPoly1Size);
	Device::registersList().add(&_traj1a0);
	Device::registersList().add(&_traj1a1);
	Device::registersList().add(&_traj1a2);
	Device::registersList().add(&_traj1a3);
	Device::registersList().add(&_traj1a4);
	Device::registersList().add(&_torquePoly1Size);
	Device::registersList().add(&_torque1a0);
	Device::registersList().add(&_torque1a1);
	Device::registersList().add(&_torque1a2);
	Device::registersList().add(&_torque1a3);
	Device::registersList().add(&_torque1a4);
	Device::registersList().add(&_duration1);
	Device::registersList().add(&_trajPoly2Size);
	Device::registersList().add(&_traj2a0);
	Device::registersList().add(&_traj2a1);
	Device::registersList().add(&_traj2a2);
	Device::registersList().add(&_traj2a3);
	Device::registersList().add(&_traj2a4);
	Device::registersList().add(&_torquePoly2Size);
	Device::registersList().add(&_torque2a0);
	Device::registersList().add(&_torque2a1);
	Device::registersList().add(&_torque2a2);
	Device::registersList().add(&_torque2a3);
	Device::registersList().add(&_torque2a4);
	Device::registersList().add(&_duration2);
	Device::registersList().add(&_mode);
	Device::registersList().add(&_copyNextBuffer);
	Device::registersList().add(&_positionTrackerOn);
	Device::registersList().add(&_debugOn);
	Device::registersList().add(&_unused);
	Device::registersList().add(&_i0);
	Device::registersList().add(&_r);
	Device::registersList().add(&_ke);
	Device::registersList().add(&_kvis);
	Device::registersList().add(&_kstat);
	Device::registersList().add(&_kcoul);
	Device::registersList().add(&_linearTransition);
	Device::registersList().add(&_speedCalculationDelay);
	Device::registersList().add(&_outputTorque);
	Device::registersList().add(&_electricalTorque);
	Device::registersList().add(&_frozenRamOn);
	Device::registersList().add(&_useValuesNow);
	Device::registersList().add(&_torqueKp);
	Device::registersList().add(&_goalTorque);
	Device::registersList().add(&_predictiveCommandPeriod);
}

void Dynaban64::onSwap()
{
    this->DXL::onSwap();
//	const TimePoint tp  = getTimePoint();
//	t = duration_float(tp);
}

/**
 * Helpers for trajectories
 */

void Dynaban64::prepareFirstTrajectory(float * positionCoefs, int nbPositionCoefs,
									float * torqueCoefs, int nbTorqueCoefs,
									float duration)
{
	float fivePositionCoefs[5];

	if (nbPositionCoefs != 0) {
    	// Common duration for position and torque trajectory
    	duration1() = duration;

    	for (int i = 0; i < nbPositionCoefs; i++) {
    		fivePositionCoefs[i] = positionCoefs[i];
    	}
    	for (int i = nbPositionCoefs; i < 5; i++) {
    		fivePositionCoefs[i] = 0.0;
    	}
    	// fivePositionCoefs contains always 5 coefs, the lasts ones are 0.0 if the user asked for less than 5 coefs.
    	setPositionTrajectory1(fivePositionCoefs[0], fivePositionCoefs[1], fivePositionCoefs[2], fivePositionCoefs[3], fivePositionCoefs[4]);
    	trajPoly1Size() = nbPositionCoefs;
	}

	//Same goes for the torque coefs, unless there are none
	if (nbTorqueCoefs != 0) {
    	for (int i = 0; i < nbTorqueCoefs; i++) {
    		fivePositionCoefs[i] = torqueCoefs[i];
    	}
    	for (int i = nbTorqueCoefs; i < 5; i++) {
    		fivePositionCoefs[i] = 0.0;
    	}
    	// fivePositionCoefs contains always 5 coefs, the lasts ones are 0.0 if the user asked for less than 5 coefs.
    	setTorqueTrajectory1(fivePositionCoefs[0], fivePositionCoefs[1], fivePositionCoefs[2], fivePositionCoefs[3], fivePositionCoefs[4]);
    	torquePoly1Size() = nbTorqueCoefs;
	}
	/*
	 * We set copy next buffer to 0, this means that if nothing is done the movement will stop at the end of the current traj.
	 *But it also means that the firmware is ready to receive a second trajectory that, if transmitted, will be used once the first one has ended.
	 */
	copyNextBuffer() = 0;
}

void Dynaban64::startFirstTrajectoryNow(int mode)
{
	this->mode() = mode;
}

bool Dynaban64::isReadyForNextTrajectory()
{
	if (copyNextBuffer() != 0) {
		return false;
	}
	return true;
}

void Dynaban64::updateNextTrajectory(float * positionCoefs, int nbPositionCoefs,
		float * torqueCoefs, int nbTorqueCoefs,
		float duration)
{
	float fivePositionCoefs[5];

	if (nbPositionCoefs != 0) {
    	// Common duration for position and torque trajectory
    	duration2() = duration;

    	for (int i = 0; i < nbPositionCoefs; i++) {
    		fivePositionCoefs[i] = positionCoefs[i];
    	}
    	for (int i = nbPositionCoefs; i < 5; i++) {
    		fivePositionCoefs[i] = 0.0;
    	}
    	// fivePositionCoefs contains always 5 coefs, the lasts ones are 0.0 if the user asked for less than 5 coefs.
    	setPositionTrajectory2(fivePositionCoefs[0], fivePositionCoefs[1], fivePositionCoefs[2], fivePositionCoefs[3], fivePositionCoefs[4]);
    	trajPoly2Size() = nbPositionCoefs;
	}

	//Same goes for the torque coefs, unless there are none
	if (nbTorqueCoefs != 0) {
    	for (int i = 0; i < nbTorqueCoefs; i++) {
    		fivePositionCoefs[i] = torqueCoefs[i];
    	}
    	for (int i = nbTorqueCoefs; i < 5; i++) {
    		fivePositionCoefs[i] = 0.0;
    	}
    	// fivePositionCoefs contains always 5 coefs, the lasts ones are 0.0 if the user asked for less than 5 coefs.
    	setTorqueTrajectory2(fivePositionCoefs[0], fivePositionCoefs[1], fivePositionCoefs[2], fivePositionCoefs[3], fivePositionCoefs[4]);
    	//TODO why do we need to comment this line???
    	//torquePoly2Size() = nbTorqueCoefs;
	}
	/*
	 * We set copy next buffer to 1, this tells dynaban to use our new trajectory once the last one has finished.
	 * The copy next buffer will be reset to 0 once dynaban starts using the new trajectory.
	 */
	copyNextBuffer() = 1;
}

void Dynaban64::stopAtTheEndOfTheTrajectory()
{
	copyNextBuffer() = 0;
}

/**
 * Special getters/setters
 */
void Dynaban64::getPositionTrajectory1(float coefs[5])
{
	coefs[0] = traj1a0();
	coefs[1] = traj1a1();
	coefs[2] = traj1a2();
	coefs[3] = traj1a3();
	coefs[4] = traj1a4();
}

void Dynaban64::setPositionTrajectory1(const float a0, const float a1, const float a2, const float a3, const float a4)
{
	traj1a0() = a0;
	traj1a1() = a1;
	traj1a2() = a2;
	traj1a3() = a3;
	traj1a4() = a4;
}


void Dynaban64::getTorqueTrajectory1(float coefs[5])
{
	coefs[0] = torque1a0();
	coefs[1] = torque1a1();
	coefs[2] = torque1a2();
	coefs[3] = torque1a3();
	coefs[4] = torque1a4();
}

void Dynaban64::setTorqueTrajectory1(const float a0, const float a1, const float a2, const float a3, const float a4)
{
	torque1a0() = a0;
	torque1a1() = a1;
	torque1a2() = a2;
	torque1a3() = a3;
	torque1a4() = a4;
}

void Dynaban64::getPositionTrajectory2(float coefs[5])
{
	coefs[0] = traj2a0();
	coefs[1] = traj2a1();
	coefs[2] = traj2a2();
	coefs[3] = traj2a3();
	coefs[4] = traj2a4();
}

void Dynaban64::setPositionTrajectory2(const float a0, const float a1, const float a2, const float a3, const float a4)
{
	traj2a0() = a0;
	traj2a1() = a1;
	traj2a2() = a2;
	traj2a3() = a3;
	traj2a4() = a4;
}

void Dynaban64::getTorqueTrajectory2(float coefs[5])
{
	coefs[0] = torque2a0();
	coefs[1] = torque2a1();
	coefs[2] = torque2a2();
	coefs[3] = torque2a3();
	coefs[4] = torque2a4();
}

void Dynaban64::setTorqueTrajectory2(const float a0, const float a1, const float a2, const float a3, const float a4)
{
	torque2a0() = a0;
	torque2a1() = a1;
	torque2a2() = a2;
	torque2a3() = a3;
	torque2a4() = a4;
}

/**
 * Registers access
 */


TypedRegisterInt& Dynaban64::trajPoly1Size()
{
    return _trajPoly1Size;
}


TypedRegisterFloat& Dynaban64::traj1a0()
{
    return _traj1a0;
}

TypedRegisterFloat& Dynaban64::traj1a1()
{
    return _traj1a1;
}

TypedRegisterFloat& Dynaban64::traj1a2()
{
    return _traj1a2;
}

TypedRegisterFloat& Dynaban64::traj1a3()
{
    return _traj1a3;
}

TypedRegisterFloat& Dynaban64::traj1a4()
{
    return _traj1a4;
}


TypedRegisterInt& Dynaban64::torquePoly1Size()
{
    return _torquePoly1Size;
}

TypedRegisterFloat& Dynaban64::torque1a0()
{
    return _torque1a0;
}

TypedRegisterFloat& Dynaban64::torque1a1()
{
    return _torque1a1;
}

TypedRegisterFloat& Dynaban64::torque1a2()
{
    return _torque1a2;
}

TypedRegisterFloat& Dynaban64::torque1a3()
{
    return _torque1a3;
}

TypedRegisterFloat& Dynaban64::torque1a4()
{
    return _torque1a4;
}


TypedRegisterFloat& Dynaban64::duration1()
{
    return _duration1;
}


TypedRegisterInt& Dynaban64::trajPoly2Size()
{
    return _trajPoly2Size;
}

TypedRegisterFloat& Dynaban64::traj2a0()
{
    return _traj2a0;
}

TypedRegisterFloat& Dynaban64::traj2a1()
{
    return _traj2a1;
}

TypedRegisterFloat& Dynaban64::traj2a2()
{
    return _traj2a2;
}

TypedRegisterFloat& Dynaban64::traj2a3()
{
    return _traj2a3;
}

TypedRegisterFloat& Dynaban64::traj2a4()
{
    return _traj2a4;
}


TypedRegisterInt& Dynaban64::torquePoly2Size()
{
    return _torquePoly2Size;
}

TypedRegisterFloat& Dynaban64::torque2a0()
{
    return _torque2a0;
}

TypedRegisterFloat& Dynaban64::torque2a1()
{
    return _torque2a1;
}

TypedRegisterFloat& Dynaban64::torque2a2()
{
    return _torque2a2;
}

TypedRegisterFloat& Dynaban64::torque2a3()
{
    return _torque2a3;
}

TypedRegisterFloat& Dynaban64::torque2a4()
{
    return _torque2a4;
}


TypedRegisterFloat& Dynaban64::duration2()
{
    return _duration2;
}


TypedRegisterInt& Dynaban64::mode()
{
    return _mode;
}

TypedRegisterInt& Dynaban64::copyNextBuffer()
{
    return _copyNextBuffer;
}

TypedRegisterBool& Dynaban64::positionTrackerOn()
{
    return _positionTrackerOn;
}

TypedRegisterBool& Dynaban64::debugOn()
{
    return _debugOn;
}

TypedRegisterInt& Dynaban64::unused()
{
    return _unused;
}


TypedRegisterFloat& Dynaban64::i0()
{
    return _i0;
}

TypedRegisterFloat& Dynaban64::r()
{
    return _r;
}

TypedRegisterFloat& Dynaban64::ke()
{
    return _ke;
}

TypedRegisterFloat& Dynaban64::kvis()
{
    return _kvis;
}

TypedRegisterFloat& Dynaban64::kstat()
{
    return _kstat;
}

TypedRegisterFloat& Dynaban64::kcoul()
{
    return _kcoul;
}

TypedRegisterFloat& Dynaban64::linearTransition()
{
    return _linearTransition;
}

TypedRegisterFloat& Dynaban64::speedCalculationDelay()
{
    return _speedCalculationDelay;
}

TypedRegisterFloat& Dynaban64::outputTorque()
{
    return _outputTorque;
}

TypedRegisterFloat& Dynaban64::electricalTorque()
{
    return _electricalTorque;
}


TypedRegisterBool& Dynaban64::frozenRamOn()
{
    return _frozenRamOn;
}

TypedRegisterBool& Dynaban64::useValuesNow()
{
    return _useValuesNow;
}

TypedRegisterInt& Dynaban64::torqueKp()
{
    return _torqueKp;
}

TypedRegisterFloat& Dynaban64::goalTorque()
{
    return _goalTorque;
}

TypedRegisterInt& Dynaban64::predictiveCommandPeriod()
{
    return _predictiveCommandPeriod;
}




}
