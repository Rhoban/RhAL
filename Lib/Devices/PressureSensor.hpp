#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <cmath>
#include <type_traits>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "types.h"

namespace RhAL {

class PressureSensorBase : public Device
{
  public:
    PressureSensorBase(const std::string& name, id_t id)
            : Device(name, id),
              callback([]{})
    {
    }

    virtual int gauges()=0;
    virtual float gain(int index)=0;
    virtual TypedRegisterInt& pressure(int index)=0;
    virtual void setZero(int index, double value)=0;

    virtual float getX()=0;
    virtual float getY()=0;
    virtual float getWeight()=0;


    virtual ReadValueFloat getXValue()=0;
    virtual ReadValueFloat getYValue()=0;
    virtual ReadValueFloat getWeightValue()=0;


    void setCallback(std::function<void()> callback_)
    {
        callback = callback_;
    }

  protected:
    /**
     * A callback that is invoked after a filtering
     */
    std::function<void()> callback;
    TimePoint timestamp;
    bool isError;
};

/**
 * PressureSensor
 */
template <int GAUGES>
class PressureSensor : public PressureSensorBase
{
  public:

    /**
     * Initialization with name and id
     */
    PressureSensor(const std::string& name, id_t id)
            : PressureSensorBase(name, id),
	      _led("led", 0x19, 1, convEncode_Bool, convDecode_Bool, 0),
              _id("id", 0x03, 1, convEncode_1Byte, convDecode_1Byte, 0, true, false, true)
    {

        for (unsigned int i=0;i<GAUGES;i++) {
            std::stringstream ss;
            ss << "pressure_" << i;
            _pressure.push_back(std::shared_ptr<TypedRegisterInt>(
                new TypedRegisterInt(ss.str(), 0x24+3*i, 3,
                                     [i, this](const data_t* data) -> int {
                                         std::lock_guard<std::mutex> lock(this->_mutex);
                                         int value = convDecode_3Bytes_signed(data);
                                         return ((double)this->_gain[i]->value)*(value - (int)this->_zero[i]->value);
                                     },
                                     1)));

            ss.str("");
            ss << "zero_" << i;
            _zero.push_back(std::shared_ptr<ParameterNumber>(new ParameterNumber(ss.str(), 0.0)));
            ss.str("");
            ss << "x_" << i;
            _x.push_back(std::shared_ptr<ParameterNumber>(new ParameterNumber(ss.str(), 0.0)));
            ss.str("");
            ss << "y_" << i;
            _y.push_back(std::shared_ptr<ParameterNumber>(new ParameterNumber(ss.str(), 0.0)));
            ss.str("");
            ss << "gain_" << i;
            _gain.push_back(std::shared_ptr<ParameterNumber>(new ParameterNumber(ss.str(), 1.0)));
        }
    }

    void onSwap()
    {
        float total = 0;
        unsigned int n = _zero.size();
        float x_ = 0;
        float y_ = 0;
        for (unsigned int k=0; k<n; k++) {
            float weight_ = pressure(k);
            if (weight_ > 0) {
                total += weight_;
                x_ += _x[k]->value*weight_;
                y_ += _y[k]->value*weight_;
            }
        }
        weight = total;
        if (weight > 1e-6) {
            x = x_/weight;
            y = y_/weight;
        } else {
            x = y = 0;
        }
        timestamp=pressure(0).readValue().timestamp;
        isError=pressure(0).readValue().isError;

        callback();
    }

    /**
     * How many gauges are managed?
     */
    int gauges()
    {
        return GAUGES;
    }

    /**
     * Gain
     */
    float gain(int index)
    {
        return _gain[index]->value;
    }

    /**
     * Registers access
     */
    TypedRegisterInt& pressure(int index)
    {
        return *_pressure[index];
    }

    /**
     * Parameters zeros get/set
     */
    float getZero(int index) const
    {
        std::lock_guard<std::mutex> lock(_mutex);

        return _zero[index]->value;
    }

    void setZero(int index, double value)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        _zero[index]->value = value;
    }

    float getX()
    {
        return x;
    }
    float getY()
    {
        return y;
    }
    float getWeight()
    {
        return weight;
    }


    ReadValueFloat getXValue()
    {
        return ReadValueFloat(timestamp,x,isError);
    }
    ReadValueFloat getYValue()
    {
        return ReadValueFloat(timestamp,y,isError);
    }
    ReadValueFloat getWeightValue()
    {
        return ReadValueFloat(timestamp,weight,isError);
    }


    /**
     * Sets the filter callback
     */
    void setCallback(std::function<void()> callback_)
    {
        callback = callback_;
    }

  protected:
        /**
         * Registers
         */
        //The following comments specify the register
        //size and address in the hardware.
        std::vector<std::shared_ptr<TypedRegisterInt>> _pressure; // Starts at 0x24

        // Parameters for calibration
        std::vector<std::shared_ptr<ParameterNumber>> _zero;
        std::vector<std::shared_ptr<ParameterNumber>> _x;
        std::vector<std::shared_ptr<ParameterNumber>> _y;
        std::vector<std::shared_ptr<ParameterNumber>> _gain;
        TypedRegisterInt _id;

        // Led
        TypedRegisterBool _led; // At 0x19

        /**
         * Parameters
         */
        float x, y, weight;

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        virtual void onInit() override
        {
            Device::registersList().add(&_id);
            Device::registersList().add(&_led);
            for (auto &reg : _pressure) {
                Device::registersList().add(reg.get());
            }
            for (unsigned int k=0; k<_zero.size(); k++) {
                Device::parametersList().add(_zero[k].get());
                Device::parametersList().add(_x[k].get());
                Device::parametersList().add(_y[k].get());
                Device::parametersList().add(_gain[k].get());
            }
        }
    };

    /**
 * DeviceManager specialized for PressureSensor
 */
template <int GAUGES>
class ImplManager<PressureSensor<GAUGES>> : public TypedManager<PressureSensor<GAUGES>>
{
public:

    inline static type_t typeNumber()
    {
        return 5000+GAUGES;
    }

    inline static std::string typeName()
    {
        std::stringstream ss;
        ss << "PressureSensor" << GAUGES;
        return ss.str();
    }
};

/**
 * Explicit template instanciation
 */
extern template class PressureSensor<4>;
extern template class PressureSensor<8>;

/**
 * Typedef
 */
typedef PressureSensor<4> PressureSensor4;
typedef PressureSensor<8> PressureSensor8;

}
