#include "ConvertionUtils.h"

namespace RhAL {

void write1ByteToBuffer(data_t* buffer, uint8_t value)
{
    *(buffer) = (value & 0xFF);
}
void write2BytesToBuffer(data_t* buffer, uint16_t value)
{
    *(buffer) = (value & 0xFF);
    *(buffer + 1) = ((value >> 8) & 0xFF);
}
void write3BytesToBuffer(data_t* buffer, uint32_t value)
{
    *(buffer) = (value & 0xFF);
    *(buffer + 1) = ((value >> 8) & 0xFF);
    *(buffer + 2) = ((value >> 16) & 0xFF);
}
void writeFloatToBuffer(data_t* buffer, float value)
{
    //Attention ! This compile time assert checks
    //the size of the float but not its endianness.
    //This implementation might fail depending on the platform.
    static_assert(sizeof(value) == 4,
        "Float is not 32 bit on this platform, I'm done !");
    //We need to do this in order to access the bits from our float
    unsigned char *c = reinterpret_cast<unsigned char *>(&value);
    *(buffer) = c[0];
    *(buffer + 1) = c[1];
    *(buffer + 2) = c[2];
    *(buffer + 3) = c[3];
}

uint8_t read1ByteFromBuffer(const data_t* buffer)
{
    uint8_t val;
    val = *(buffer);
    return val;
}
uint16_t read2BytesFromBuffer(const data_t* buffer)
{
    uint16_t val = 0;
    val = (*(buffer + 1) << 8) | (*(buffer));
    return val;
}
uint32_t read3BytesFromBuffer(const data_t* buffer)
{
    uint32_t val = 0;
    val = (*(buffer + 2) << 16) | (*(buffer + 1) << 8) | (*(buffer));
    return val;
}
float readFloatFromBuffer(const data_t* buffer)
{
    //Attention ! To be tested
    uint32_t temp = (*(buffer + 3) << 24) |
        (*(buffer + 2) << 16) | (*(buffer + 1) << 8) | (*(buffer));
    float* val = reinterpret_cast<float*>(&temp);

    return *val;
}

void convEncode_Bool(data_t* buffer, bool value)
{
    if (value) {
        write1ByteToBuffer(buffer, (uint8_t)1);
    } else {
        write1ByteToBuffer(buffer, (uint8_t)0);
    }
}

void convEncode_1Byte(data_t* buffer, uint8_t value)
{
    write1ByteToBuffer(buffer, value);
}

void convEncode_2Bytes(data_t* buffer, uint16_t value)
{
    write2BytesToBuffer(buffer, value);
}

void convEncode_3Bytes(data_t* buffer, uint32_t value)
{
    write3BytesToBuffer(buffer, value);
}

void convEncode_float(data_t* buffer, float value)
{
    writeFloatToBuffer(buffer, value);
}

bool convDecode_Bool(const data_t* buffer)
{
    uint8_t value = read1ByteFromBuffer(buffer);
    bool result = true;
    if (value == 0) {
        result = false;
    }

    return result;
}

uint8_t convDecode_1Byte(const data_t* buffer)
{
    return read1ByteFromBuffer(buffer);
}

uint16_t convDecode_2Bytes(const data_t* buffer)
{
    return read2BytesFromBuffer(buffer);
}

uint32_t convDecode_3Bytes(const data_t* buffer)
{
    return read3BytesFromBuffer(buffer);
}

#define VALUE_SIGN(value, length) \
        ((value < (1<<(length-1))) ? \
         (value) \
         : (value-(1<<length)))

int8_t convDecode_1Byte_signed(const data_t* buffer)
{
    return VALUE_SIGN(read1ByteFromBuffer(buffer), 8);
}

int16_t convDecode_2Bytes_signed(const data_t* buffer)
{
    return VALUE_SIGN(read2BytesFromBuffer(buffer), 16);
}

int32_t convDecode_3Bytes_signed(const data_t* buffer)
{
    return VALUE_SIGN(read3BytesFromBuffer(buffer), 24);
}

float convDecode_float(const data_t* buffer)
{
    return readFloatFromBuffer(buffer);
}

}

