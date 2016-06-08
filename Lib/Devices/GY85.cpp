#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/GY85.hpp"

// This is the gain to get g from LSB
// From the ADXL324 documentation, the accelerometer is 256 LSB
// per g
#define ACC_GAIN (1.0/256.0)

// This is the gain to get rad/s from LSB
// From ITG-3200 documentation, the gyroscope is 14.375 LSB
// per °/s
#define GYRO_GAIN (M_PI/(180.0*14.375))

namespace RhAL {

static float accelerometerDecode(const data_t *data)
{
    return ACC_GAIN*convDecode_2Bytes_signed(data);
}

static float gyroscopeDecode(const data_t *data)
{
    return GYRO_GAIN*convDecode_2Bytes_signed(data);
}

GY85::GY85(const std::string& name, id_t id) :
    Device(name, id),
    filter(false),
    compassFilter(true),
    callback([]{}),
    sequence(0)
{
    for (int k=0; k<GY85_VALUES; k++) {
        size_t offset = k*(9*2 + 4);
        std::stringstream ss;
        std::string prefix;
        ss << "acc_" << k;
        prefix = ss.str();

        values[k].accX = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat(prefix+"_x",
                    0x24 +offset, 2, accelerometerDecode, 1));
        values[k].accY = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat(prefix+"_y",
                    0x26 +offset, 2, accelerometerDecode, 1));
        values[k].accZ = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat(prefix+"_z",
                    0x28 +offset, 2, accelerometerDecode, 1));

        ss.str("");
        ss << "gyro_" << k;
        prefix = ss.str();

        values[k].gyroX = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat(prefix+"_x",
                    0x2a +offset, 2, gyroscopeDecode, 1));
        values[k].gyroY = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat(prefix+"_y",
                    0x2c +offset, 2, gyroscopeDecode, 1));
        values[k].gyroZ = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat(prefix+"_z",
                    0x2e +offset, 2, gyroscopeDecode, 1));

        ss.str("");
        ss << "magn_" << k;
        prefix = ss.str();

        values[k].magnX = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat(prefix+"_x",
                    0x30 +offset, 2, convDecode_2Bytes_signed, 1));
        values[k].magnY = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat(prefix+"_y",
                    0x32 +offset, 2, convDecode_2Bytes_signed, 1));
        values[k].magnZ = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat(prefix+"_z",
                    0x34 +offset, 2, convDecode_2Bytes_signed, 1));

        ss.str("");
        ss << "sequence_" << k;
        values[k].sequence = std::shared_ptr<TypedRegisterInt>(new TypedRegisterInt(ss.str(),
                    0x36 +offset, 4, convDecode_4Bytes, 1));
    }

    _kp_rollpitch = std::shared_ptr<ParameterNumber>(new ParameterNumber("kp_rollpitch", 0.02));
    _ki_rollpitch = std::shared_ptr<ParameterNumber>(new ParameterNumber("ki_rollpitch", 0.00002));

    _invertOrientation = std::shared_ptr<ParameterBool>(new ParameterBool("invertOrientation", false));

    _gyroXOffset = std::shared_ptr<ParameterNumber>(new ParameterNumber("gyroXOffset", 0.0));
    _gyroYOffset = std::shared_ptr<ParameterNumber>(new ParameterNumber("gyroYOffset", 0.0));
    _gyroZOffset = std::shared_ptr<ParameterNumber>(new ParameterNumber("gyroZOffset", 0.0));

    _accXMin = std::shared_ptr<ParameterNumber>(new ParameterNumber("accXMin", -1.0));
    _accXMax = std::shared_ptr<ParameterNumber>(new ParameterNumber("accXMax", 1.0));
    _accYMin = std::shared_ptr<ParameterNumber>(new ParameterNumber("accYMin", -1.0));
    _accYMax = std::shared_ptr<ParameterNumber>(new ParameterNumber("accYMax", 1.0));
    _accZMin = std::shared_ptr<ParameterNumber>(new ParameterNumber("accZMin", -1.0));
    _accZMax = std::shared_ptr<ParameterNumber>(new ParameterNumber("accZMax", 1.0));

    _magnXMin = std::shared_ptr<ParameterNumber>(new ParameterNumber("magnXMin", -100.0));
    _magnXMax = std::shared_ptr<ParameterNumber>(new ParameterNumber("magnXMax", 100.0));
    _magnYMin = std::shared_ptr<ParameterNumber>(new ParameterNumber("magnYMin", -100.0));
    _magnYMax = std::shared_ptr<ParameterNumber>(new ParameterNumber("magnYMax", 100.0));
    _magnZMin = std::shared_ptr<ParameterNumber>(new ParameterNumber("magnZMin", -100.0));
    _magnZMax = std::shared_ptr<ParameterNumber>(new ParameterNumber("magnZMax", 100.0));
    _filterDelay = std::shared_ptr<ParameterNumber>(new ParameterNumber("filterDelay", 0.016));

}

void GY85::onInit()
{
    for (int k=0; k<GY85_VALUES; k++) {
        Device::registersList().add(values[k].accX.get());
        Device::registersList().add(values[k].accY.get());
        Device::registersList().add(values[k].accZ.get());
        Device::registersList().add(values[k].gyroX.get());
        Device::registersList().add(values[k].gyroY.get());
        Device::registersList().add(values[k].gyroZ.get());
        Device::registersList().add(values[k].magnX.get());
        Device::registersList().add(values[k].magnY.get());
        Device::registersList().add(values[k].magnZ.get());
        Device::registersList().add(values[k].sequence.get());
    }
    Device::parametersList().add(_kp_rollpitch.get());
    Device::parametersList().add(_ki_rollpitch.get());
    Device::parametersList().add(_invertOrientation.get());
    Device::parametersList().add(_gyroXOffset.get());
    Device::parametersList().add(_gyroYOffset.get());
    Device::parametersList().add(_gyroZOffset.get());
    Device::parametersList().add(_accXMin.get());
    Device::parametersList().add(_accXMax.get());
    Device::parametersList().add(_accYMin.get());
    Device::parametersList().add(_accYMax.get());
    Device::parametersList().add(_accZMin.get());
    Device::parametersList().add(_accZMax.get());
    Device::parametersList().add(_magnXMin.get());
    Device::parametersList().add(_magnXMax.get());
    Device::parametersList().add(_magnYMin.get());
    Device::parametersList().add(_magnYMax.get());
    Device::parametersList().add(_magnZMin.get());
    Device::parametersList().add(_magnZMax.get());
    Device::parametersList().add(_filterDelay.get());

}

void GY85::setCallback(std::function<void()> callback_)
{
    callback = callback_;
}

void GY85::setGyroCalibration(float x, float y, float z)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _gyroXOffset->value = x;
    _gyroYOffset->value = y;
    _gyroZOffset->value = z;
}

float GY85::getAccX()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return accX;
}
float GY85::getAccY()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return accY;
}
float GY85::getAccZ()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return accZ;
}

float GY85::getGyroX()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return gyroX;
}
float GY85::getGyroY()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return gyroY;
}
float GY85::getGyroZ()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return gyroZ;
}

float GY85::getMagnX()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return magnX;
}
float GY85::getMagnY()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return magnY;
}
float GY85::getMagnZ()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return magnZ;
}

float GY85::getAccXRaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return accXRaw;
}
float GY85::getAccYRaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return accYRaw;
}
float GY85::getAccZRaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return accZRaw;
}

float GY85::getGyroXRaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return gyroXRaw;
}
float GY85::getGyroYRaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return gyroYRaw;
}
float GY85::getGyroZRaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return gyroZRaw;
}

float GY85::getMagnXRaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return magnXRaw;
}
float GY85::getMagnYRaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return magnYRaw;
}
float GY85::getMagnZRaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return magnZRaw;
}

//Filters


float GY85::getGyroYaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return filter.gyroYaw;
}
float GY85::getYaw()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return filter.yaw;
}
float GY85::getPitch()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return filter.pitch;
}
float GY85::getRoll()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return filter.roll;
}

float GY85::getYawCompass()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return compassFilter.yaw;
}
float GY85::getPitchCompass()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return compassFilter.pitch;
}
float GY85::getRollCompass()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return compassFilter.roll;
}
float GY85::getMagnAzimuth()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return compassFilter.magnAzimuth;
}
float GY85::getMagnHeading()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return compassFilter.magnHeading;
}


//Timestamped


ReadValueFloat GY85::getGyroYawValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValueFloat(timestamp,filter.gyroYaw,isError);
}
ReadValueFloat GY85::getYawValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValueFloat(timestamp,filter.yaw,isError);
}
ReadValueFloat GY85::getPitchValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValueFloat(timestamp,filter.pitch,isError);
}
ReadValueFloat GY85::getRollValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValueFloat(timestamp,filter.roll,isError);
}

ReadValueFloat GY85::getYawCompassValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValueFloat(timestamp,compassFilter.yaw,isError);
}
ReadValueFloat GY85::getPitchCompassValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValueFloat(timestamp,compassFilter.pitch,isError);
}
ReadValueFloat GY85::getRollCompassValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValueFloat(timestamp,compassFilter.roll,isError);
}
ReadValueFloat GY85::getMagnAzimuthValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValueFloat(timestamp,compassFilter.magnAzimuth,isError);
}
ReadValueFloat GY85::getMagnHeadingValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValueFloat(timestamp,compassFilter.magnHeading,isError);
}




Eigen::Matrix3d GY85::getMatrix()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return filter.getMatrix();
}
Eigen::Matrix3d GY85::getMatrixCompass()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return compassFilter.getMatrix();
}

inline static float compensation(float value, float min, float max)
{
    float offset = (min+max)/2;
    float range = (max-min)/2;

    return (value-offset)/range;
}

void GY85::onSwap()
{
    _mutex.lock();

    // Scanning all the values, pos will be the index of the
    // current value if it is found, or the smallest sequence
    // if not
    int older = -1;
    int newer = -1;
    int currentPos = -1;
    for (int k=0; k<GY85_VALUES; k++) {
        uint32_t seq = (uint32_t)values[k].sequence->readValue().value;
        if (seq == sequence) {
            currentPos = k;
        }
        if (older < 0 || seq < (uint32_t)values[older].sequence->readValue().value) {
            older = k;
        }
        if (newer < 0 || seq > (uint32_t)values[newer].sequence->readValue().value) {
            newer = k;
        }
    }
    if (currentPos < 0 && sequence != 0) {
        // XXX: The current sequence was not found in the buffers,
        // we may have missed some values
        // We assume thus that the current pos is the older value
        std::cerr << "[GY-85] Missed packets, is the frequency too low?" << std::endl;
    }

    timestamp=values[newer].sequence->readValue().timestamp;
    timestamp-=TimeDurationMicro(long(_filterDelay->value*1000000)); //take into account the filter delay

    isError=values[newer].sequence->readValue().isError;

    bool updated = false;
    while (currentPos != newer) {
        if (currentPos < 0) {
            currentPos = older;
        } else {
            currentPos++;
            if (currentPos >= GY85_VALUES) {
                currentPos = 0;
            }
        }

        // Update current values
        accXRaw = values[currentPos].accX->readValue().value;
        accYRaw = values[currentPos].accY->readValue().value;
        accZRaw = values[currentPos].accZ->readValue().value;
        gyroXRaw = values[currentPos].gyroX->readValue().value;
        gyroYRaw = values[currentPos].gyroY->readValue().value;
        gyroZRaw = values[currentPos].gyroZ->readValue().value;
        magnXRaw = values[currentPos].magnX->readValue().value;
        magnYRaw = values[currentPos].magnY->readValue().value;
        magnZRaw = values[currentPos].magnZ->readValue().value;
        sequence = (uint32_t)values[currentPos].sequence->readValue().value;

        // XXX: Apply calibration
        accX = compensation(accXRaw, _accXMin->value, _accXMax->value);
        accY = compensation(accYRaw, _accYMin->value, _accYMax->value);
        accZ = compensation(accZRaw, _accZMin->value, _accZMax->value);
        gyroX = gyroXRaw - _gyroXOffset->value;
        gyroY = gyroYRaw - _gyroYOffset->value;
        gyroZ = gyroZRaw - _gyroZOffset->value;
        magnX = compensation(magnXRaw, _magnXMin->value, _magnXMax->value);
        magnY = compensation(magnYRaw, _magnYMin->value, _magnYMax->value);
        magnZ = compensation(magnZRaw, _magnZMin->value, _magnZMax->value);

        updated = true;

        // Updating filters
        compassFilter.accel[0] = filter.accel[0] = accX;
        compassFilter.accel[1] = filter.accel[1] = accY;
        compassFilter.accel[2] = filter.accel[2] = accZ;
        compassFilter.gyro[0] = filter.gyro[0] = gyroX;
        compassFilter.gyro[1] = filter.gyro[1] = gyroY;
        compassFilter.gyro[2] = filter.gyro[2] = gyroZ;
        compassFilter.magnetom[0] = filter.magnetom[0] = magnX;
        compassFilter.magnetom[1] = filter.magnetom[1] = magnY;
        compassFilter.magnetom[2] = filter.magnetom[2] = magnZ;
        compassFilter.Kp_rollPitch = filter.Kp_rollPitch = _kp_rollpitch->value;
        compassFilter.Ki_rollPitch = filter.Ki_rollPitch = _ki_rollpitch->value;

        if(!isError) //Do not update filter if the last read was an error????? Should not happen...
        {
            filter.update();
            compassFilter.update();
        }

        filter.invert = _invertOrientation->value;
        compassFilter.invert = _invertOrientation->value;
    }

    _mutex.unlock();
    if (updated) {
        callback();
    }
}

}
