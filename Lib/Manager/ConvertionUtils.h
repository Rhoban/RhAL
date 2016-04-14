#pragma once

#include "types.h"

namespace RhAL {

/**
 * Write to given data buffer
 */
void write1ByteToBuffer(data_t* buffer, uint8_t value);
void write2BytesToBuffer(data_t* buffer, uint16_t value);
void write3BytesToBuffer(data_t* buffer, uint32_t value);
void writeFloatToBuffer(data_t* buffer, float value);

/**
 * Read from buffer
 */
uint8_t read1ByteFromBuffer(const data_t* buffer);
uint16_t read2BytesFromBuffer(const data_t* buffer);
uint32_t read3BytesFromBuffer(const data_t* buffer);
float readFloatFromBuffer(const data_t* buffer);

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
void convEncode_Bool(data_t* buffer, bool value);

/**
 * Default 1Byte encode (raw copy)
 */
void convEncode_1Byte(data_t* buffer, uint8_t value);

/**
 * Default 2Bytes encode (raw copy)
 */
void convEncode_2Bytes(data_t* buffer, uint16_t value);

/**
 * Default 3Bytes encode (raw copy)
 */
void convEncode_3Bytes(data_t* buffer, uint32_t value);

/**
 * Default float encode (raw copy)
 */
void convEncode_float(data_t* buffer, float value);

/**
 * Default bool decode (raw copy)
 */
bool convDecode_Bool(const data_t* buffer);

/**
 * Default 1Byte decode (raw copy)
 */
uint8_t convDecode_1Byte(const data_t* buffer);
int8_t convDecode_1Byte_signed(const data_t* buffer);

/**
 * Default 2Bytes decode (raw copy)
 */
uint16_t convDecode_2Bytes(const data_t* buffer);
int16_t convDecode_2Bytes_signed(const data_t* buffer);

/**
 * Default 3Bytes decode (raw copy)
 */
uint32_t convDecode_3Bytes(const data_t* buffer);
int32_t convDecode_3Bytes_signed(const data_t* buffer);

/**
 * Default 4Bytes decode (raw copy)
 */
uint32_t convDecode_4Bytes(const data_t* buffer);
int32_t convDecode_4Bytes_signed(const data_t* buffer);

/**
 * Default float decode (raw copy)
 */
float convDecode_float(const data_t* buffer);

}

