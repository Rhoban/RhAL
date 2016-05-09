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
void convEncode_PolyDuration(data_t* buffer, float value);
/**
 * Decode function for polynomial duration. Output in seconds (precision 0.0001 s)
 */
float convDecode_PolyDuration(const data_t* buffer);

/**
 * Encode function for polynomial duration. Input in seconds (precision 0.001 s)
 */
void convEncode_speedDelay(data_t* buffer, float value);
/**
 * Decode function for polynomial duration. Output in seconds (precision 0.001 s)
 */
float convDecode_speedDelay(const data_t* buffer);

/**
 * Encode function for polynomial coefficients. Input in degrees.
 */
void convEncode_positionTraj(data_t* buffer, float positionInRads);

/**
 * Decode function for polynomial coefficients. Output in degrees.
 */
float convDecode_positionTraj(const data_t* buffer);


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
        Dynaban64(const std::string& name, id_t id);

        /**
         * Helpers for trajectories
         */
        /**
         * This function will send to the servo the first position trajectory and torque trajectory with their durations.
         * Units are degrees and seconds
         * Once this is done, call startFirstTrajectoryNow to start the trajectory.
         * If you want the trajectory to be continued by an other one call updateNextTrajectory.
         */
        void prepareFirstTrajectory(float * positionCoefs, int nbPositionCoefs,
        									float * torqueCoefs, int nbTorqueCoefs,
											float duration);

        /*As of 25/02/2016 :
		 *1 : Predictive command only. Follows the trajectory set in the traj1 fields but only relying on the model of the motor. This mode can be useful when calibrating the model
		 *2 : PID only. Follows the trajectory set in the traj1 fields but only relying on the PID.
		 *3 : PID and predictive command. Follows the trajectory set in the traj1 fields using both the PID and the predictive command. This should be the default mode when following a trajectory
		 */
        void startFirstTrajectoryNow(int mode);

        bool isReadyForNextTrajectory();

    	/*
    	 * This function should be called only if copyNextBuffer is false, which means that Dynaban is ready to receive a trajectory (that will be bufferized until the current
    	 * trajectory is finished).
    	 * The only situation where it'd make sense to call this function if copyNextBuffer is true is if we've sent a continuation trajectory
    	 * that hasn't yet been used and we want to change it before Dynaban finishes its current traj. Ofc, not recommended.
    	 */
        void updateNextTrajectory(float * positionCoefs, int nbPositionCoefs,
				float * torqueCoefs, int nbTorqueCoefs,
				float duration);

        /**
         * Tells the servo to stop once the current trajectory is over. The calls to updateNextTrajectory set the copyNextBuffer flag to 1,
         * if stopAtTheEndOfTheTrajectory is not called, the servo will swap the buffers one last time, replaying the penultimate trajectory [disastrous].
         */
        void stopAtTheEndOfTheTrajectory();



        /**
         * Special getters/setters
         */
		/**
		 * Fills an array of floats containing the coefficients of the position trajectory1 : [a0, a1, a2, a3, a4]
		 */
		void getPositionTrajectory1(float coefs[5]);
		/**
		 * Sets the 5 coefficients of the polynomial position trajectory 1
		 */
		void setPositionTrajectory1(const float a0, const float a1, const float a2, const float a3, const float a4);

		/**
		 * Fills an array of floats containing the coefficients of the torque trajectory1 : [a0, a1, a2, a3, a4]
		 */
		void getTorqueTrajectory1(float coefs[5]);
		/**
		 * Sets the 5 coefficients of the polynomial torque trajectory 1
		 */
		void setTorqueTrajectory1(const float a0, const float a1, const float a2, const float a3, const float a4);

		/**
		 * Fills an array of floats containing the coefficients of the position trajectory2 : [a0, a1, a2, a3, a4]
		 */
		void getPositionTrajectory2(float coefs[5]);
		/**
		 * Sets the 5 coefficients of the polynomial position trajectory 2
		 */
		void setPositionTrajectory2(const float a0, const float a1, const float a2, const float a3, const float a4);

		/**
		 * Fills an array of floats containing the coefficients of the torque trajectory2 : [a0, a1, a2, a3, a4]
		 */
		void getTorqueTrajectory2(float coefs[5]);
		/**
		 * Sets the 5 coefficients of the polynomial position trajectory 1
		 */
		void setTorqueTrajectory2(const float a0, const float a1, const float a2, const float a3, const float a4);

        /**
         * Registers access
         */

		/**
		 * Size of the position trajectory polynomial. 5 is the maximum value.
		 * e.g if set to 3, the firmware will only consider a0, a1, a2.
		 */
		TypedRegisterInt& trajPoly1Size();

		TypedRegisterFloat& traj1a0();
		TypedRegisterFloat& traj1a1();
		TypedRegisterFloat& traj1a2();
		TypedRegisterFloat& traj1a3();
		TypedRegisterFloat& traj1a4();

		/**
		 * Size of the torque trajectory polynomial. 5 is the maximum value.
		 * e.g if set to 3, the firmware will only consider a0, a1, a2.
		 */
		TypedRegisterInt& torquePoly1Size();
		TypedRegisterFloat& torque1a0();
		TypedRegisterFloat& torque1a1();
		TypedRegisterFloat& torque1a2();
		TypedRegisterFloat& torque1a3();
		TypedRegisterFloat& torque1a4();

        /**
         * Duration in s for position and torque polynomials 1.
         */
		TypedRegisterFloat& duration1();

		/**
		 * Position trajectory polynomial 2. 5 is the maximum value.
		 * e.g if set to 3, the firmware will only consider a0, a1, a2.
		 */
		TypedRegisterInt& trajPoly2Size();
		TypedRegisterFloat& traj2a0();
		TypedRegisterFloat& traj2a1();
		TypedRegisterFloat& traj2a2();
		TypedRegisterFloat& traj2a3();
		TypedRegisterFloat& traj2a4();

		/**
		 * Sets the size of the torque trajectory polynomial 2. 5 is the maximum value.
		 * e.g if set to 3, the firmware will only consider a0, a1, a2.
		 */
		TypedRegisterInt& torquePoly2Size();
		TypedRegisterFloat& torque2a0();
		TypedRegisterFloat& torque2a1();
		TypedRegisterFloat& torque2a2();
		TypedRegisterFloat& torque2a3();
		TypedRegisterFloat& torque2a4();

        /**
         * Duration in s for position and torque polynomials 2.
         */
		TypedRegisterFloat& duration2();

		/**
		 * Value of the Mode field. As of 25/02/2016 :
		 *0 : Default mode. Uses the PID to follow the goal position. The behaviour should be almost identical to the default firmware
		 *1 : Predictive command only. Follows the trajectory set in the traj1 fields but only relying on the model of the motor. This mode can be useful when calibrating the model
		 *2 : PID only. Follows the trajectory set in the traj1 fields but only relying on the PID.
		 *3 : PID and predictive command. Follows the trajectory set in the traj1 fields using both the PID and the predictive command. This should be the default mode when following a trajectory
		 *4 : Compliant-kind-of mode. In this mode, the servo will try to act compliant
		 */
		TypedRegisterInt& mode();
		TypedRegisterInt& copyNextBuffer();
		TypedRegisterBool& positionTrackerOn();
		TypedRegisterBool& debugOn();
		TypedRegisterInt& unused();

        /**
         * Moment of inertia of the motor in N.m with only its gearbox attached to it
         */
		TypedRegisterFloat& i0();
        /**
         * Resistance of the motor in ohms
         */
		TypedRegisterFloat& r();
        /**
         * ke in V.s/rad
         */
		TypedRegisterFloat& ke();
        /**
         * kvis in N.m*s/rad
         */
		TypedRegisterFloat& kvis();
        /**
         * kstat in N.m
         */
		TypedRegisterFloat& kstat();
        /**
         * kcoul in N.m
         */
		TypedRegisterFloat& kcoul();
        /**
         * linearTransition in rad/s
         */
		TypedRegisterFloat& linearTransition();
        /**
         * speedCalculationDelay in s
         */
		TypedRegisterFloat& speedCalculationDelay();
        /**
         * outputTorque in N.m
         */
		TypedRegisterFloat& outputTorque();
        /**
         * electricalTorque in N.m
         */
		TypedRegisterFloat& electricalTorque();

		TypedRegisterBool& frozenRamOn();
		TypedRegisterBool& useValuesNow();
		TypedRegisterInt& torqueKp();
		/**
		 * Goal torque in N.m
		 */
		TypedRegisterFloat& goalTorque();
		TypedRegisterInt& predictiveCommandPeriod();


    protected :
        /**
         * Inherit.
         * Declare Registers and parameters
         */
        virtual void onInit() override;

        virtual void onSwap() override;

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
		TypedRegisterInt 	_predictiveCommandPeriod;	//1 D6
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

