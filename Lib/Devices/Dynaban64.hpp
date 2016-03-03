#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/MX64.hpp"
#include "types.h"

namespace RhAL {

/**
 * Encode function for polynomial duration. Input in seconds (precision 0.0001 s)
 */
inline void convEncode_PolyDuration(data_t* buffer, float value)
{
	if (value < 0.0) {
		value = 0.0;
	} else if (value > 5.6) {
		value = 5.6;
	}

	uint16_t result = std::lround(value*10000);
	write2BytesToBuffer(buffer, result);
}
/**
 * Decode function for polynomial duration. Output in seconds (precision 0.0001 s)
 */
inline float convDecode_PolyDuration(const data_t* buffer)
{
	uint16_t val = read2BytesFromBuffer(buffer);
	float result = (float)val/10000.0;

	return result;
}

/**
 * Encode function for polynomial duration. Input in seconds (precision 0.001 s)
 */
inline void convEncode_speedDelay(data_t* buffer, float value)
{
	if (value < 0.0) {
		value = 0.0;
	} else if (value > 5.6) {
		value = 5.6;
	}

	uint16_t result = std::lround(value*1000);
	write2BytesToBuffer(buffer, result);
}
/**
 * Decode function for polynomial duration. Output in seconds (precision 0.001 s)
 */
inline float convDecode_speedDelay(const data_t* buffer)
{
	uint16_t val = read2BytesFromBuffer(buffer);
	float result = (float)val/1000.0;

	return result;
}

/**
 * Encode function for polynomial coefficients. Input in rads.
 */
inline void convEncode_positionTraj(data_t* buffer, float positionInRads) {
	float result = positionInRads * 4096/(2*M_PI);
	writeFloatToBuffer(buffer, result);
}

/**
 * Decode function for polynomial coefficients. Output in rads.
 */
inline float convDecode_positionTraj(const data_t* buffer) {
	float val = readFloatFromBuffer(buffer);
	float result = val*2*M_PI/4096.0;

	return result;
}

/**
 * Dynaban64
 *
 * Dynaban64 Device
 * implementation
 */
class Dynaban64 : public MX64
{
    public:

        /**
         * Initialization with name and id
         */
        inline Dynaban64(const std::string& name, id_t id) :
            MX64(name, id),
			_trajPoly1Size("trajPoly1Size", 0x4A, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_traj1a0("traj1a0", 0x4B, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_traj1a1("traj1a1", 0x4F, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_traj1a2("traj1a2", 0x53, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_traj1a3("traj1a3", 0x57, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_traj1a4("traj1a4", 0x5B, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_torquePoly1Size("torquePoly1Size", 0x5F, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_torque1a0("torque1a0", 0x60, 4, convEncode_float, convDecode_float, 0),
			_torque1a1("torque1a1", 0x64, 4, convEncode_float, convDecode_float, 0),
			_torque1a2("torque1a2", 0x68, 4, convEncode_float, convDecode_float, 0),
			_torque1a3("torque1a3", 0x6C, 4, convEncode_float, convDecode_float, 0),
			_torque1a4("torque1a4", 0x70, 4, convEncode_float, convDecode_float, 0),
			_duration1("duration1", 0x74, 2, convEncode_PolyDuration, convDecode_PolyDuration, 0),
			_trajPoly2Size("trajPoly2Size", 0x76, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_traj2a0("traj2a0", 0x77, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_traj2a1("traj2a1", 0x7B, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_traj2a2("traj2a2", 0x7F, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_traj2a3("traj2a3", 0x83, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_traj2a4("traj2a4", 0x87, 4, convEncode_positionTraj, convDecode_positionTraj, 0),
			_torquePoly2Size("torquePoly2Size", 0x8B, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_torque2a0("torque2a0", 0x8C, 4, convEncode_float, convDecode_float, 0),
			_torque2a1("torque2a1", 0x90, 4, convEncode_float, convDecode_float, 0),
			_torque2a2("torque2a2", 0x94, 4, convEncode_float, convDecode_float, 0),
			_torque2a3("torque2a3", 0x98, 4, convEncode_float, convDecode_float, 0),
			_torque2a4("torque2a4", 0x9C, 4, convEncode_float, convDecode_float, 0),
			_duration2("duration2", 0xA0, 2, convEncode_PolyDuration, convDecode_PolyDuration, 0),

			_mode("mode", 0xA2, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_copyNextBuffer("copyNextBuffer", 0xA3, 1, convEncode_1Byte, convDecode_1Byte, 0),
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
			_outputTorque("ouputTorque", 0xC6, 4, convDecode_float, 0), // Read only
			_electricalTorque("electricalTorque", 0xCA, 4, convDecode_float, 0), // Read Only
			_frozenRamOn("frozenRamOn", 0xCE, 1, convEncode_Bool, convDecode_Bool, 0),
			_useValuesNow("useValuesNow", 0xCF, 1, convEncode_Bool, convDecode_Bool, 0),
			_torqueKp("torqueKp", 0xD0, 1, convEncode_2Bytes, convDecode_2Bytes, 0),
			_goalTorque("goalTorque", 0xD2, 4, convEncode_float, convDecode_float, 0)

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

        /**
         * Helpers for trajectories
         */
        /**
         * This function will send to the servo the first position trajectory and torque trajectory with their durations.
         * Units are SI (rads, seconds)
         * Once this is done, call startFirstTrajectoryNow to start the trajectory.
         * If you want the trajectory to be continued by an other one call updateNextTrajectory.
         */
        inline void prepareFirstTrajectory(float * positionCoefs, int nbPositionCoefs,
        									float * torqueCoefs, int nbTorqueCoefs,
											float duration)
        {
        	// Common duration for position and torque trajectory
        	setDuration1(duration);

        	float fivePositionCoefs[5];
        	for (int i = 0; i < nbPositionCoefs; i++) {
        		fivePositionCoefs[i] = positionCoefs[i];
        	}
        	for (int i = nbPositionCoefs; i < 5; i++) {
        		fivePositionCoefs[i] = 0.0;
        	}
        	// fivePositionCoefs contains always 5 coefs, the lasts ones are 0.0 if the user asked for less than 5 coefs.
        	setPositionTrajectory1(fivePositionCoefs[0], fivePositionCoefs[1], fivePositionCoefs[2], fivePositionCoefs[3], fivePositionCoefs[4]);
        	setPositionTrajectory1Size(nbPositionCoefs);

        	//Same goes for the torque coefs, unless there are none
        	if (nbTorqueCoefs != 0) {
            	for (int i = 0; i < nbTorqueCoefs; i++) {
            		fivePositionCoefs[i] = torqueCoefs[i];
            	}
            	for (int i = nbTorqueCoefs; i < 5; i++) {
            		fivePositionCoefs[i] = 0.0;
            	}
            	// fivePositionCoefs contains always 5 coefs, the lasts ones are 0.0 if the user asked for less than 5 coefs.
            	setPositionTrajectory1(fivePositionCoefs[0], fivePositionCoefs[1], fivePositionCoefs[2], fivePositionCoefs[3], fivePositionCoefs[4]);
            	setPositionTrajectory1Size(nbTorqueCoefs);
        	}
        	/*
        	 * We set copy next buffer to 0, this means that if nothing is done the movement will stop at the end of the current traj.
        	 *But it also means that the firmware is ready to receive a second trajectory that, if transmitted, will be used once the first one has ended.
        	 */
        	setCopyNextBuffer(0);
        }

        /*As of 25/02/2016 :
		 *1 : Predictive command only. Follows the trajectory set in the traj1 fields but only relying on the model of the motor. This mode can be useful when calibrating the model
		 *2 : PID only. Follows the trajectory set in the traj1 fields but only relying on the PID.
		 *3 : PID and predictive command. Follows the trajectory set in the traj1 fields using both the PID and the predictive command. This should be the default mode when following a trajectory
		 */
        inline void startFirstTrajectoryNow(int mode)
        {
        	setMode(mode);
        }

        inline bool isReadyForNextTrajectory()
        {
        	if (getCopyNextBuffer()) {
        		return false;
        	}
        	return true;
        }

    	/*
    	 * This function should be called only if copyNextBuffer is false, which means that Dynaban is ready to receive a trajectory (that will be bufferized until the current
    	 * trajectory is finished).
    	 * The only situation where it'd make sense to call this function if copyNextBuffer is true is if we've sent a continuation trajectory
    	 * that hasn't yet been used and we want to change it before Dynaban finishes its current traj. Ofc, not recommended.
    	 */
        inline void updateNextTrajectory(float * positionCoefs, int nbPositionCoefs,
				float * torqueCoefs, int nbTorqueCoefs,
				float duration)
        {

        	// Common duration for position and torque trajectory
        	setDuration2(duration);

        	float fivePositionCoefs[5];
        	for (int i = 0; i < nbPositionCoefs; i++) {
        		fivePositionCoefs[i] = positionCoefs[i];
        	}
        	for (int i = nbPositionCoefs; i < 5; i++) {
        		fivePositionCoefs[i] = 0.0;
        	}
        	// fivePositionCoefs contains always 5 coefs, the lasts ones are 0.0 if the user asked for less than 5 coefs.
        	setPositionTrajectory2(fivePositionCoefs[0], fivePositionCoefs[1], fivePositionCoefs[2], fivePositionCoefs[3], fivePositionCoefs[4]);
        	setPositionTrajectory2Size(nbPositionCoefs);

        	//Same goes for the torque coefs, unless there are none
        	if (nbTorqueCoefs != 0) {
            	for (int i = 0; i < nbTorqueCoefs; i++) {
            		fivePositionCoefs[i] = torqueCoefs[i];
            	}
            	for (int i = nbTorqueCoefs; i < 5; i++) {
            		fivePositionCoefs[i] = 0.0;
            	}
            	// fivePositionCoefs contains always 5 coefs, the lasts ones are 0.0 if the user asked for less than 5 coefs.
            	setPositionTrajectory2(fivePositionCoefs[0], fivePositionCoefs[1], fivePositionCoefs[2], fivePositionCoefs[3], fivePositionCoefs[4]);
            	setPositionTrajectory2Size(nbTorqueCoefs);
        	}
        	/*
        	 * We set copy next buffer to 1, this tells dynaban to use our new trajectory once the last one has finished.
        	 * The copy next buffer will be reset to 0 once dynaban starts using the new trajectory.
        	 */
        	setCopyNextBuffer(1);
        }


        /**
         * Getters of hell.
         */


		/**
		 * Sets the size of the position trajectory polynomial. 5 is the maximum value.
		 * e.g if set to 3, the firmware will only consider a0, a1, a2.
		 */
		inline void setPositionTrajectory1Size(const int value)
		{
			_trajPoly1Size.writeValue(value);
		}
		/**
		 * Fills an array of floats containing the coefficients of the position trajectory1 : [a0, a1, a2, a3, a4]
		 */
		inline void getPositionTrajectory1(float coefs[5])
		{
			coefs[0] = _traj1a0.readValue().value;
			coefs[1] = _traj1a1.readValue().value;
			coefs[2] = _traj1a2.readValue().value;
			coefs[3] = _traj1a3.readValue().value;
			coefs[4] = _traj1a4.readValue().value;
		}
		/**
		 * Sets the 5 coefficients of the polynomial position trajectory 1
		 */
		inline void setPositionTrajectory1(const float a0, const float a1, const float a2, const float a3, const float a4)
		{
			_traj1a0.writeValue(a0);
			_traj1a1.writeValue(a1);
			_traj1a2.writeValue(a2);
			_traj1a3.writeValue(a3);
			_traj1a4.writeValue(a4);
		}


		inline int getTorqueTrajectory1Size()
		{
			return _torquePoly1Size.readValue().value;
		}
		/**
		 * Sets the size of the torque trajectory polynomial. 5 is the maximum value.
		 * e.g if set to 3, the firmware will only consider a0, a1, a2.
		 */
		inline void setTorqueTrajectory1Size(const int value)
		{
			_torquePoly1Size.writeValue(value);
		}
		/**
		 * Fills an array of floats containing the coefficients of the torque trajectory1 : [a0, a1, a2, a3, a4]
		 */
		inline void getTorqueTrajectory1(float coefs[5])
		{
			coefs[0] = _torque1a0.readValue().value;
			coefs[1] = _torque1a1.readValue().value;
			coefs[2] = _torque1a2.readValue().value;
			coefs[3] = _torque1a3.readValue().value;
			coefs[4] = _torque1a4.readValue().value;
		}
		/**
		 * Sets the 5 coefficients of the polynomial torque trajectory 1
		 */
		inline void setTorqueTrajectory1(const float a0, const float a1, const float a2, const float a3, const float a4)
		{
			_torque1a0.writeValue(a0);
			_torque1a1.writeValue(a1);
			_torque1a2.writeValue(a2);
			_torque1a3.writeValue(a3);
			_torque1a4.writeValue(a4);
		}

        /**
         * Gets the duration in s for position and torque polynomials 1.
         */
		inline float getDuration1()
		{
			return _duration1.readValue().value;
		}
		/**
         * Sets the duration in s for position and torque polynomials 1.
   		 */
		inline void setDuration1(const float value)
		{
			_duration1.writeValue(value);
		}


		inline int getPositionTrajectory2Size()
		{
			return _trajPoly2Size.readValue().value;
		}
		/**
		 * Sets the size of the position trajectory polynomial 2. 5 is the maximum value.
		 * e.g if set to 3, the firmware will only consider a0, a1, a2.
		 */
		inline void setPositionTrajectory2Size(const int value)
		{
			_trajPoly2Size.writeValue(value);
		}

		/**
		 * Fills an array of floats containing the coefficients of the position trajectory2 : [a0, a1, a2, a3, a4]
		 */
		inline void getPositionTrajectory2(float coefs[5])
		{
			coefs[0] = _traj2a0.readValue().value;
			coefs[1] = _traj2a1.readValue().value;
			coefs[2] = _traj2a2.readValue().value;
			coefs[3] = _traj2a3.readValue().value;
			coefs[4] = _traj2a4.readValue().value;
		}
		/**
		 * Sets the 5 coefficients of the polynomial position trajectory 2
		 */
		inline void setPositionTrajectory2(const float a0, const float a1, const float a2, const float a3, const float a4)
		{
			_traj2a0.writeValue(a0);
			_traj2a1.writeValue(a1);
			_traj2a2.writeValue(a2);
			_traj2a3.writeValue(a3);
			_traj2a4.writeValue(a4);
		}

		inline int getTorqueTrajectory2Size()
		{
			return _torquePoly2Size.readValue().value;
		}
		/**
		 * Sets the size of the torque trajectory polynomial 2. 5 is the maximum value.
		 * e.g if set to 3, the firmware will only consider a0, a1, a2.
		 */
		inline void setTorqueTrajectory2Size(const int value)
		{
			_torquePoly2Size.writeValue(value);
		}

		/**
		 * Fills an array of floats containing the coefficients of the torque trajectory2 : [a0, a1, a2, a3, a4]
		 */
		inline void getTorqueTrajectory2(float coefs[5])
		{
			coefs[0] = _torque2a0.readValue().value;
			coefs[1] = _torque2a1.readValue().value;
			coefs[2] = _torque2a2.readValue().value;
			coefs[3] = _torque2a3.readValue().value;
			coefs[4] = _torque2a4.readValue().value;
		}
		/**
		 * Sets the 5 coefficients of the polynomial position trajectory 1
		 */
		inline void setTorqueTrajectory2(const float a0, const float a1, const float a2, const float a3, const float a4)
		{
			_torque2a0.writeValue(a0);
			_torque2a1.writeValue(a1);
			_torque2a2.writeValue(a2);
			_torque2a3.writeValue(a3);
			_torque2a4.writeValue(a4);
		}


        /**
         * Gets the duration in s for position and torque polynomials 2.
         */
		inline float getDuration2()
		{
			return _duration2.readValue().value;
		}
		/**
         * Sets the duration in s for position and torque polynomials 2.
   		 */
		inline void setDuration2(const float value)
		{
			_duration1.writeValue(value);
		}

		/**
		 * Returns the value of the Mode field. As of 25/02/2016 :
		 *0 : Default mode. Uses the PID to follow the goal position. The behaviour should be almost identical to the default firmware
		 *1 : Predictive command only. Follows the trajectory set in the traj1 fields but only relying on the model of the motor. This mode can be useful when calibrating the model
		 *2 : PID only. Follows the trajectory set in the traj1 fields but only relying on the PID.
		 *3 : PID and predictive command. Follows the trajectory set in the traj1 fields using both the PID and the predictive command. This should be the default mode when following a trajectory
		 *4 : Compliant-kind-of mode. In this mode, the servo will try to act compliant
		 */
		inline int getMode()
		{
			return _mode.readValue().value;
		}
		/**
		 * Sets the value of the Mode field. As of 25/02/2016 :
		 *0 : Default mode. Uses the PID to follow the goal position. The behaviour should be almost identical to the default firmware
		 *1 : Predictive command only. Follows the trajectory set in the traj1 fields but only relying on the model of the motor. This mode can be useful when calibrating the model
		 *2 : PID only. Follows the trajectory set in the traj1 fields but only relying on the PID.
		 *3 : PID and predictive command. Follows the trajectory set in the traj1 fields using both the PID and the predictive command. This should be the default mode when following a trajectory
		 *4 : Compliant-kind-of mode. In this mode, the servo will try to act compliant
		 */
		inline void setMode(const int mode)
		{
			_mode.writeValue(mode);
		}

		inline int getCopyNextBuffer()
		{
			return _copyNextBuffer.readValue().value;
		}
		inline void setCopyNextBuffer(int value)
		{
			return _copyNextBuffer.writeValue(value);
		}

		inline bool getPositionTrackerOn()
		{
			return _positionTrackerOn.readValue().value;
		}
		inline void setPositionTrackerOn(bool value)
		{
			return _positionTrackerOn.writeValue(value);
		}

		inline bool getDebugOn()
		{
			return _debugOn.readValue().value;
		}
		inline void setDebugOn(bool value)
		{
			return _debugOn.writeValue(value);
		}

		inline int getUnused()
		{
			return _unused.readValue().value;
		}
		inline void setUnused(int value)
		{
			return _unused.writeValue(value);
		}

        /**
         * Gets the moment of inertia of the motor in N.m with only its gearbox attached to it
         */
		inline float getI0()
		{
			return _i0.readValue().value;
		}
		/**
         * Sets the moment of inertia of the motor in N.m with only its gearbox attached to it
   		 */
		inline void setI0(const float value)
		{
			_i0.writeValue(value);
		}

        /**
         * Gets the resistance of the motor in ohms
         */
		inline float getR()
		{
			return _r.readValue().value;
		}
		/**
         * Sets the resistance of the motor in ohms
   		 */
		inline void setR(const float value)
		{
			_r.writeValue(value);
		}

        /**
         * Gets ke in V.s/rad
         */
		inline float getKe()
		{
			return _ke.readValue().value;
		}
		/**
         * Sets ke in V.s/rad
   		 */
		inline void setKe(const float value)
		{
			_ke.writeValue(value);
		}

        /**
         * Gets kvis in N.m*s/rad
         */
		inline float getKvis()
		{
			return _kvis.readValue().value;
		}
		/**
         * Sets kvis in N.m*s/rad
   		 */
		inline void setKvis(const float value)
		{
			_kvis.writeValue(value);
		}

        /**
         * Gets kstat in N.m
         */
		inline float getKstat()
		{
			return _kstat.readValue().value;
		}
		/**
         * Sets kstat in N.m
   		 */
		inline void setKstat(const float value)
		{
			_kstat.writeValue(value);
		}

        /**
         * Gets kcoul in N.m
         */
		inline float getKcoul()
		{
			return _kcoul.readValue().value;
		}
		/**
         * Sets kcoul in N.m
   		 */
		inline void setKcoul(const float value)
		{
			_kcoul.writeValue(value);
		}

        /**
         * Gets linearTransition in rad/s
         */
		inline float getLinearTransition()
		{
			return _linearTransition.readValue().value;
		}
		/**
         * Sets linearTransition in rad/
   		 */
		inline void setLinearTransition(const float value)
		{
			_linearTransition.writeValue(value);
		}

        /**
         * Gets speedCalculationDelay in s
         */
		inline float getSpeedCalculationDelay()
		{
			return _speedCalculationDelay.readValue().value;
		}
		/**
         * Sets speedCalculationDelay in s
   		 */
		inline void setSpeedCalculationDelay(const float value)
		{
			_speedCalculationDelay.writeValue(value);
		}

        /**
         * Gets outputTorque in N.m
         */
		inline float getOutputTorque()
		{
			return _outputTorque.readValue().value;
		}

        /**
         * Gets electricalTorque in N.m
         */
		inline float getElectricalTorque()
		{
			return _electricalTorque.readValue().value;
		}


		inline bool getFrozenRamOn()
		{
			return _frozenRamOn.readValue().value;
		}
		inline void setFrozenRamOn(const bool value)
		{
			_frozenRamOn.writeValue(value);
		}

		inline bool getUseValuesNow()
		{
			return _useValuesNow.readValue().value;
		}
		inline void setUseValuesNow(const bool value)
		{
			_useValuesNow.writeValue(value);
		}

		inline int getTorqueKp()
		{
			return _torqueKp.readValue().value;
		}
		inline void setTorqueKp(const int value)
		{
			_torqueKp.writeValue(value);
		}

		/**
		 * Gets the goal torque in N.m
		 */
		inline float getGoalTorque()
		{
			return _goalTorque.readValue().value;
		}
		/**
		 * Sets the goal torque in N.m
		 */
		inline void setGoalTorque(const float value)
		{
			_goalTorque.writeValue(value);
		}

    protected :
        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
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
        }

        inline virtual void onSwap() override
       	{
        	const TimePoint tp  = getTimePoint();
        	t = duration_float(tp);

  		}
		// Dynaban specific registers :

		TypedRegisterInt	_trajPoly1Size;				//1 4A
		TypedRegisterFloat  _traj1a0;					//4 4B
		TypedRegisterFloat  _traj1a1;					//4 4F
		TypedRegisterFloat  _traj1a2;					//4 53
		TypedRegisterFloat  _traj1a3;					//4 57
		TypedRegisterFloat  _traj1a4;					//4 5B

		TypedRegisterInt	_torquePoly1Size;			//1 5F
		TypedRegisterFloat  _torque1a0;					//4 60
		TypedRegisterFloat  _torque1a1;					//4 64
		TypedRegisterFloat  _torque1a2;					//4 68
		TypedRegisterFloat  _torque1a3;					//4 6C
		TypedRegisterFloat  _torque1a4;					//4 70

		TypedRegisterFloat	_duration1;					//2 75

		TypedRegisterInt	_trajPoly2Size;				//1 76
		TypedRegisterFloat  _traj2a0;					//4 77
		TypedRegisterFloat  _traj2a1;					//4 7B
		TypedRegisterFloat  _traj2a2;					//4 7F
		TypedRegisterFloat  _traj2a3;					//4 83
		TypedRegisterFloat  _traj2a4;					//4 87

		TypedRegisterInt	_torquePoly2Size;			//1 8B
		TypedRegisterFloat  _torque2a0;					//4 8C
		TypedRegisterFloat  _torque2a1;					//4 90
		TypedRegisterFloat  _torque2a2;					//4 94
		TypedRegisterFloat  _torque2a3;					//4 98
		TypedRegisterFloat  _torque2a4;					//4 9C

		TypedRegisterFloat	_duration2;					//2 A0

		TypedRegisterInt	_mode;						//1 A2
		TypedRegisterInt	_copyNextBuffer;			//1 A3
		TypedRegisterBool	_positionTrackerOn;			//1 A4
		TypedRegisterBool	_debugOn;					//1 A5
		TypedRegisterInt	_unused;					//2 A6

		TypedRegisterFloat  _i0;						//4 A8
		TypedRegisterFloat  _r;							//4 AC
		TypedRegisterFloat  _ke;						//4 B0
		TypedRegisterFloat  _kvis;						//4 B4
		TypedRegisterFloat  _kstat;						//4 B8
		TypedRegisterFloat  _kcoul;						//4 BC
		TypedRegisterFloat  _linearTransition;			//4 C0
		TypedRegisterFloat	_speedCalculationDelay;		//2 C4
		TypedRegisterFloat  _outputTorque;				//4 C6
		TypedRegisterFloat  _electricalTorque;			//4 CA

		TypedRegisterBool	_frozenRamOn;				//1 CE
		TypedRegisterBool	_useValuesNow;				//1 CF
		TypedRegisterInt	_torqueKp;					//2 D0
		TypedRegisterFloat  _goalTorque;				//4 D2
};

/**
 * DeviceManager specialized for Dynaban64
 */
template <>
class ImplManager<Dynaban64> : public TypedManager<Dynaban64>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x0136;
        }

        inline static std::string typeName()
        {
            return "Dynaban64";
        }
};

}

