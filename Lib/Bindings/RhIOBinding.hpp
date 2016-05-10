#pragma once

#include <thread>
#include <vector>
#include <string>
#include <RhIO.hpp>

namespace RhAL {

class BaseManager;
class ParametersList;
class Device;

/**
 * RhIOBinding
 *
 * Binding with Rhoban/RhIO user
 * interface system
 */
class RhIOBinding
{
    public:

        /**
         * Initialization of RhIO Binding.
         * manager: main RhAL Manager instance.
         * nodeName: RhIO node name for RhAL.
         * isUpdateThread: if true, a thread periodicaly
         * check for new Devices/Parameters.
         */
        RhIOBinding(
            BaseManager& manager, 
            const std::string& nodeName = "lowlevel",
            bool isUpdateThread = true);

        /**
         * Destructor
         */
        virtual ~RhIOBinding();

        /**
         * Update the binding with
         * current Manager state.
         * (Update Devices parameters and
         * check for new Devices/parameeters)
         */
        void update();

        /**
         * Doing specific updates
         * on float registers.
         */
        void specificUpdate(
            RhIO::IONode *node, RhAL::Device *device);

    private:

        /**
         * Binding Thread
         */
        std::thread* _thread;
        bool _isOver;

        /**
         * Main Manager instance 
         */
        BaseManager* _manager;

        /**
         * Pointer to RhIO node
         */
        RhIO::IONode* _node;

        /**
         * Update RhIO on given RhAL ParameterList
         */
        void updateParameters(
            ParametersList& params,
            RhIO::IONode* node);

        /**
         * RhIO scan command
         */
        std::string cmdScan(
            std::vector<std::string> argv);
        std::string cmdStats(
            std::vector<std::string> argv);
        std::string cmdResetStats(
            std::vector<std::string> argv);
        std::string cmdReadDev(
            std::vector<std::string> argv);
        std::string cmdReadReg(
            std::vector<std::string> argv);
        std::string cmdPing(
            std::vector<std::string> argv);
        std::string cmdStatus(
            std::vector<std::string> argv);
        std::string cmdCheck(
            std::vector<std::string> argv);
        std::string cmdSaveConf(
            std::vector<std::string> argv);
        std::string cmdLoadConf(
            std::vector<std::string> argv);
        std::string cmdEmergency(
            std::vector<std::string> argv);
        std::string cmdEmergencyExit(
            std::vector<std::string> argv);
        std::string cmdInit(
            std::vector<std::string> argv);
        std::string cmdChangeId(
            std::vector<std::string> argv);
        std::string cmdTare(
            std::vector<std::string> argv);
};

}

