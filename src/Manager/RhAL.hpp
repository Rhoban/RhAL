#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <stdexcept>

namespace RhAL {


/**
 * TODO
 * Module de stats
 * Module de logging custom vac user choix du format
 * Dans le Manager, gérer les broadcast
 */

class Manager
{
    public:

        /**
         *
         */
        void addDev();

        void flushWrite();
        void flushRead();

        /**
         * Get a device by its name
         * Throw a std::logic_error exception in case
         * of invalid given derived device type
         */
        template<typename T>
        const T& get(const std::string& name) const;
        template<typename T>
        T& get(const std::string& name);
        template<typename T>
        const T& get(id_t id) const;
        template<typename T>
        T& get(id_t id);

    private:
    
        /**
         * Devices container name to device and id to device
         */
        std::unordered_map<std::string, Device*> _devicesName;
        std::unordered_map<id_t, Device*> _devicesId;
};

class Device
{
    public:

        /**
         * Initialization with name, 
         * id and model number
         */
        Device(const std::string& name, id_t id, type_t type);

        /**
         * Free all added registers
         */
        virtual ~Device();

        /**
         * Return the device name
         */
        const std::string& name() const;

        /**
         * Return the device id
         */
        id_t id() const;

        /**
         * Return the device model number
         */
        type_t type() const:

        /**
         * Define a new register and return
         * it newly created index.
         * Register desallocation is done by the Device
         * destructor
         */
        size_t addReg(Register* reg);

        /**
         * Return the number of contained registers
         */
        size_t sizeReg() const;

        /**
         * Access to given register index
         */
        const Register& getReg(size_t index) const;
        Register& getReg(size_t index);

    private:

        /**
         * Device unique name
         */
        std::string _name;

        /**
         * Device unique id
         */
        id_t _id;

        /**
         * Model number
         */
        type_t _type;

        /**
         * Allocated registers container
         */
        std::vector<Register*> _registers;
};

class MX64 : public Device
{
    public:

        /**
         * Register initialization
         */
        MX64() :
            Device()
        {
            _indexRegAngle = Device::addReg(Register::create(
                "angle", 0x4242, 4, true));
            _indexRegTemperature = Device::addReg(Register::create(
                "temperature", 0x4246, 4, true));
            _indexRegTarget = Device::addReg(Register::create(
                "target", 0x4300, 4, false));
        }

    private:

        /**
         * Register indexes
         */
        size_t _indexRegAngle;
        size_t _indexRegTemperature;
        size_t _indexRegTarget;
};

}

/**
 * Usage
 */
inline void test()
{
}

