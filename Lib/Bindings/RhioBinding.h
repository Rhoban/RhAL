#pragma once

#include <thread>
#include <string>
#include <RhIO.hpp>

namespace RhAL
{
    class BaseManager;
    class RhioBinding
    {
        public:
            RhioBinding(BaseManager *manager, std::string node="lowlevel");
            virtual ~RhioBinding();
            std::vector<Device*> getDevices();

            void runScheduler();
            void update();

        protected:
            bool over;
            std::thread *thread;
            BaseManager *manager;
            RhIO::Bind bind;

            // RhIO commands
            std::string cmdScan();
            std::string cmdReadDev(std::string name);
    };
}
