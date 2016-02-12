#pragma once

#include <string>
#include <mutex>
#include "types.h"
#include "Parameter.hpp"
#include "ParametersList.hpp"
#include "Register.hpp"
#include "RegistersList.hpp"
#include "CallManager.hpp"

namespace RhAL {

/**
 * Write to given data buffer
 */
inline void write1ByteToBuffer(data_t* buffer, uint8_t value)
{
    *(buffer) = (value & 0xFF);
}
inline void write2BytesToBuffer(data_t* buffer, uint16_t value)
{
    *(buffer) = (value & 0xFF);
    *(buffer + 1) = ((value >> 8) & 0xFF);
}
inline void write3BytesToBuffer(data_t* buffer, uint32_t value)
{
    *(buffer) = (value & 0xFF);
    *(buffer + 1) = ((value >> 8) & 0xFF);
    *(buffer + 2) = ((value >> 16) & 0xFF);
}
inline void writeFloatToBuffer(data_t* buffer, float value)
{
	// Attention ! This compile time assert checks the size of the float but not its endianness. This implementation might fail depending on the platform.
	static_assert(sizeof(value) == 4, "Float is not 32 bit on this platform, I'm done !");
	// We need to do this in order to access the bits from our float
	unsigned char *c = reinterpret_cast<unsigned char *>(&value);
    *(buffer) = c[0];
    *(buffer + 1) = c[1];
    *(buffer + 2) = c[2];
    *(buffer + 3) = c[3];
}

/**
 * Read from buffer
 */
inline uint8_t read1ByteFromBuffer(const data_t* buffer)
{
    uint8_t val;
    val = *(buffer);
    return val;
}
inline uint16_t read2BytesFromBuffer(const data_t* buffer)
{
    uint16_t val = 0;
    val = (*(buffer + 1) << 8) | (*(buffer));
    return val;
}
inline uint32_t read3BytesFromBuffer(const data_t* buffer)
{
    uint32_t val = 0;
    val = (*(buffer + 2) << 16) | (*(buffer + 1) << 8) | (*(buffer));
    return val;
}
inline float readFloatFromBuffer(const data_t* buffer)
{
    // Attention ! To be tested
    uint32_t temp = (*(buffer + 3) << 24) | (*(buffer + 2) << 16) | (*(buffer + 1) << 8) | (*(buffer));
    float* val = reinterpret_cast<float*>(&temp);

    return *val;
}

/**
 * Conversion functions
 */
/**
 * Default raw copy conversions. Since the raw value is contained in the hardware :
 * - "encode" is the conversion from the user to the hardware
 * - "decode" is the conversion from the hardware to the user
 */
/**
 * Default bool encode (raw copy)
 */
inline void convEncode_Bool(data_t* buffer, bool value)
{
	if (value) {
		write1ByteToBuffer(buffer, (uint8_t)1);
	} else {
		write1ByteToBuffer(buffer, (uint8_t)0);
	}
}
/**
 * Default 1Byte encode (raw copy)
 */
inline void convEncode_1Byte(data_t* buffer, uint8_t value)
{
	write1ByteToBuffer(buffer, value);
}
/**
 * Default 2Bytes encode (raw copy)
 */
inline void convEncode_2Bytes(data_t* buffer, uint16_t value)
{
	write2BytesToBuffer(buffer, value);
}
/**
 * Default 3Bytes encode (raw copy)
 */
inline void convEncode_3Bytes(data_t* buffer, uint32_t value)
{
	write3BytesToBuffer(buffer, value);
}
/**
 * Default float encode (raw copy)
 */
inline void convEncode_float(data_t* buffer, float value)
{
	write1ByteToBuffer(buffer, value);
}

/**
 * Default bool decode (raw copy)
 */
inline bool convDecode_Bool(const data_t* buffer)
{
	uint8_t value = read1ByteFromBuffer(buffer);
	bool result = true;
	if (value == 0) {
		result = false;
	}

	return result;
}
/**
 * Default 1Byte decode (raw copy)
 */
inline uint8_t convDecode_1Byte(const data_t* buffer)
{
	return read1ByteFromBuffer(buffer);
}
/**
 * Default 2Bytes decode (raw copy)
 */
inline uint16_t convDecode_2Bytes(const data_t* buffer)
{
	return read2BytesFromBuffer(buffer);
}
/**
 * Default 3Bytes decode (raw copy)
 */
inline uint32_t convDecode_3Bytes(const data_t* buffer)
{
	return read2BytesFromBuffer(buffer);
}
/**
 * Default float decode (raw copy)
 */
inline float convDecode_float(const data_t* buffer)
{
	return readFloatFromBuffer(buffer);
}


/**
 * Device
 *
 * Base class for hardware devices.
 * Hold parameters and registers pointer
 * contained in derived class.
 *
 * All parameters and registers are expected to
 * be declared in the constructor of the
 * derived Device class.
 */
class Device
{
    public:

        /**
         * Initialization with 
         * device name and id
         */
        inline Device(const std::string& name, id_t id) :
            _mutex(),
            _registersList(id),
            _parametersList(),
            _name(name),
            _id(id),
            _manager(nullptr),
            _isPresent(false)
        {
            if (id < IdDevBegin || id > IdDevEnd) {
                throw std::logic_error(
                    "Device id is outside static range: " 
                    + name);
            }
        }

        /**
         * Virtual destructor
         */
        inline virtual ~Device()
        {
        }

        /**
         * Copy constructor and 
         * assignement are forbidden
         */
        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        /**
         * Set the Manager pointer
         */
        inline void setManager(CallManager* manager)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (manager == nullptr) {
                throw std::logic_error(
                    "Device null manager pointer: "
                    + _name);
            }
            _manager = manager;
            _registersList.setManager(_manager);
        }

        /**
         * Run derived class registers and
         * parameters initialization
         */
        inline void init()
        {
            if (_manager == nullptr) {
                throw std::logic_error(
                    "Device null manager pointer: "
                    + _name);
            }
            //Call 
            onInit();
        }

        /**
         * Return the device name
         */
        inline const std::string& name() const
        {
            return _name;
        }

        /**
         * Return the device id
         */
        inline id_t id() const
        {
            return _id;
        }

        /**
         * Return true if the device has 
         * been see and is supposed 
         * enable currently on the bus
         */
        inline bool isPresent() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _isPresent;
        }

        /**
         * Read/Write access to Registers and
         * Parameters list
         */
        const RegistersList& registersList() const
        {
            return _registersList;
        }
        RegistersList& registersList()
        {
            return _registersList;
        }
        const ParametersList& parametersList() const
        {
            return _parametersList;
        }
        ParametersList& parametersList()
        {
            return _parametersList;
        }
        
    protected:

        /**
         * Mutex protecting Device state access
         */
        mutable std::mutex _mutex;

        /**
         * Set Device isPresent state.
         * (Used for friend Manager access)
         */
        inline void setPresent(bool isPresent)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _isPresent = isPresent;
        }

        /**
         * Call during device initialization.
         * Registers and Parameters are supposed
         * to be declared here
         */
        virtual void onInit() = 0;

        /**
         * Callback to be override
         * called at the begin of each
         * flush() after swapRead()
         */
        virtual inline void onSwap()
        {
            //Empty default
        }

        /**
         * Manager have access to listed 
         * Parameters and Registers
         */
        template <typename ... T>
        friend class Manager;

    private:
        
        /**
         * Register pointers container
         */
        RegistersList _registersList;

        /**
         * Container of bool, number and 
         * string device parameters
         */
        ParametersList _parametersList;
        
        /**
         * Device unique name
         */
        std::string _name;

        /**
         * Device unique id on the bus
         */
        id_t _id;

        /**
         * Pointer to the base class Manager
         * that will be provided to RegistersList
         */
        CallManager* _manager;

        /**
         * If true, the device has been see
         * and is supposed enable currently
         * on the bus
         */
        bool _isPresent;
};

}

