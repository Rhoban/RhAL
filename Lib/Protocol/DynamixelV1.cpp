#include <stdexcept>
#include <cstdio>
#include <string.h>
#include "DynamixelV1.hpp"

using namespace std;

namespace RhAL
{
    DynamixelV1::Packet::Packet(id_t id, DynamixelV1Command instruction, size_t parameters_)
    {
        position = 0;
        parameters = parameters_;

        buffer = new uint8_t[getSize()];
        buffer[2] = id;
        buffer[3] = parameters + 2;
        buffer[4] = instruction;
    }

    DynamixelV1::Packet::Packet(id_t id, size_t parameters_)
    {
        position = 0;
        parameters = parameters_;

        buffer = new uint8_t[getSize()];
        buffer[2] = id;
        buffer[3] = parameters + 2;
    }

    DynamixelV1::Packet::~Packet()
    {
        delete[] buffer;
    }

    void DynamixelV1::Packet::append(uint8_t byte)
    {
        if (position < parameters) {
            buffer[5 + position] = byte;
            position++;
        }
    }

    void DynamixelV1::Packet::append(uint8_t *data, size_t size)
    {
        if (position+size <= parameters) {
            memcpy(buffer + 5 + position, data, size);
            position += size;
        }
    }

    uint8_t DynamixelV1::Packet::computeChecksum()
    {
        uint8_t checksum = 0;
        for (size_t k=2; k<5+parameters; k++) {
            checksum += buffer[k];
        }

        return ~checksum;
    }
                
    void DynamixelV1::Packet::prepare()
    {
        buffer[0] = 0xff;
        buffer[1] = 0xff;
        buffer[5 + parameters] = computeChecksum();
    }

    void DynamixelV1::Packet::setError(uint8_t error)
    {
        buffer[4] = error;
    }
    
    uint8_t DynamixelV1::Packet::getError()
    {
        return buffer[4];
    }

    size_t DynamixelV1::Packet::getSize()
    {
        // A packet is: Header (2), ID, length, instruction, parameters and checksum
        return 2 + 1 + 1 + 1 + parameters + 1;
    }

    uint8_t *DynamixelV1::Packet::getParameters()
    {
        return buffer + 5;
    }

    DynamixelV1::DynamixelV1(Bus &bus)
        : Protocol(bus)
    {
    }

    void DynamixelV1::writeData(id_t id, addr_t address, 
            uint8_t *data, size_t size)
    {
        Packet packet(id, CommandWrite, size+1);
        packet.append(address);
        packet.append(data, size);
        sendPacket(packet);
    }

    ResponseState DynamixelV1::readData(id_t id, addr_t address, 
            uint8_t *data, size_t size)
    {
        Packet packet(id, CommandRead, 2);
        packet.append(address);
        packet.append(size);
        sendPacket(packet);

        Packet *response;
        auto code = receivePacket(response, id);
        if (code & ResponseOK) {
            memcpy(data, response->getParameters(), size);
            delete response;
        }
        return code;
    }

    bool DynamixelV1::ping(id_t id)
    {
        Packet packet(id, CommandRead, 0);
        sendPacket(packet);

        Packet *response;
        auto code = receivePacket(response, id);
        if (code & ResponseOK) {
            delete response;
            return true;
        } else {
            return false;
        }
    }

    std::vector<ResponseState> DynamixelV1::syncRead(
            const std::vector<id_t>& ids, addr_t address,
            const std::vector<uint8_t*>& datas, size_t size)
    {
        (void)ids;
        (void)address;
        (void)datas;
        (void)size;
        throw runtime_error("Sync read is not yet supported");
    }

    void DynamixelV1::syncWrite(
            const std::vector<id_t>& ids, addr_t address,
            const std::vector<uint8_t*>& datas, size_t size)
    {
        if (ids.size() != datas.size()) {
            throw runtime_error("ids and datas should have the same size() for syncWrite");
        }

        size_t N = ids.size();
        Packet packet(Broadcast, CommandSyncWrite, 2+N*(size+1));
        packet.append(address);
        packet.append(size);
        for (size_t k=0; k<N; k++) {
            packet.append(ids[k]);
            packet.append(datas[k], size);
        }
        sendPacket(packet);
    }

    void DynamixelV1::sendPacket(Packet &packet)
    {
        bus.flush();
        packet.prepare();
        bus.sendData(packet.buffer, packet.getSize());
    }

    ResponseState DynamixelV1::receivePacket(Packet* &response, id_t id, double timeout)
    {
        response = NULL;
        double start = getTimeDouble();
        size_t position = 0;

        while (getTimeDouble()-start <= timeout) {
            double t = timeout-(getTimeDouble()-start);
            if (bus.waitForData(t)) {
                size_t n = bus.available();
                uint8_t data[n];
                bus.readData(data, n);
                for (size_t k=0; k<n; k++) {
                    uint8_t byte = data[k];
                    switch (position) {
                        case 0:
                        case 1:
                            if (byte != 0xff) {
                                return ResponseBadProtocol;
                            }
                            break;
                        case 2:
                            if (byte != id) {
                                return ResponseBadId;
                            }
                            break;
                        case 3:
                            if (byte < 2) {
                                return ResponseBadSize;
                            } else {
                                response = new Packet(id, byte-2);
                            }
                            break;
                        case 4:
                            response->setError(byte);
                            break;
                        default:
                            if (position-5 < response->parameters) {
                                response->append(byte);
                            } else {
                                if (response->computeChecksum() == byte) {
                                    uint8_t error = response->getError();

                                    if (error & ErrorChecksum) {
                                        delete response;
                                        return ResponseDeviceBadChecksum;
                                    } else if (error & ErrorInstruction) {
                                        delete response;
                                        return ResponseDeviceBadInstruction;
                                    } else {
                                        unsigned int code = ResponseOK;
                                        if (error & ErrorVoltage) code |= ResponseBadVoltage;
                                        if (error & ErrorOverheat) code |= ResponseOverheat;
                                        if (error & ErrorOverload) code |= ResponseOverload;

                                        return code;
                                    }
                                } else {
                                    delete response;
                                    return ResponseBadChecksum;
                                }
                            }
                    }
                    position++;
                }
            }

            return ResponseQuiet;
        }
    }
}
