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
    // We store negated acceleration, for compatibility with the AHRS format
    return -ACC_GAIN*convDecode_2Bytes_signed(data);
}

static float gyroscopeDecode(const data_t *data)
{
    return GYRO_GAIN*convDecode_2Bytes_signed(data);
}

GY85::GY85(const std::string& name, id_t id) :
    Device(name, id),
    filter(false),
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
}
        
void GY85::setCallback(std::function<void()> callback_)
{
    callback = callback_;
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
        accX = values[currentPos].accX->readValue().value;
        accY = values[currentPos].accY->readValue().value;
        accZ = values[currentPos].accZ->readValue().value;
        gyroX = values[currentPos].gyroX->readValue().value;
        gyroY = values[currentPos].gyroY->readValue().value;
        gyroZ = values[currentPos].gyroZ->readValue().value;
        magnX = values[currentPos].magnX->readValue().value;
        magnY = values[currentPos].magnY->readValue().value;
        magnZ = values[currentPos].magnZ->readValue().value;
        sequence = (uint32_t)values[currentPos].sequence->readValue().value;
        
        updated = true;

        // Updating filter
        filter.accel[0] = accX;
        filter.accel[1] = accY;
        filter.accel[2] = accZ;
        filter.gyro[0] = gyroX;
        filter.gyro[1] = gyroY;
        filter.gyro[2] = gyroZ;
        filter.magnetom[0] = magnX;
        filter.magnetom[1] = magnY;
        filter.magnetom[2] = magnZ;
        filter.update();
    }

    _mutex.unlock();
    if (updated) {
        callback();
    }
}

}

