#pragma once

#include <string>
#include <RhIO.hpp>

namespace RhAL
{
    class Manager;
    class RhioBinding
    {
        public:
            RhioBinding(Manager *manager, std::string node="lowlevel");

            void update();

        protected:
            Manager *manager;
            RhIO::IONode *node;

            // RhIO commands
            std::string cmdScan();
    };
}
