#pragma once

#include <json.hpp>
#include "AggregateManager.hpp"

namespace RhAL {

/**
 * Manager
 *
 * Main RhAL interface
 * on top of inherited classes where
 * real implementation takes place
 */
template <typename ... Types> 
class Manager : public AggregateManager<Types...>
{
    public:

        /**
         * Initialization
         */
        inline Manager() :
            AggregateManager<Types...>()
        {
        }
        
        /**
         * Export as json object all Parameters
         * and derived Devices configurations.
         */
        inline nlohmann::json saveJSON() const
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            nlohmann::json j = this->saveAggregatedJSON();
            j["Manager"] = this->_parametersList.saveJSON();
            return j;
        }

        /**
         * Import from given json object all
         * Parameters and derived Devices configuration.
         * Throw std::runtime_error if 
         * given json is malformated.
         */
        inline void loadJSON(const nlohmann::json& j)
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            if (
                !j.is_object() ||
                j.size() != sizeof...(Types) + 1 ||
                j.count("Manager") != 1
            ) {
                throw std::runtime_error(
                    "Manager load parameters root json malformed");
            }
            this->loadAggregatedJSON(j);
            this->_parametersList.loadJSON(j.at("Manager"));
            //Reset low level communication (bus/protocol)
            this->initBus();
        }

        /**
         * Export all Parameters and Devices 
         * configuration into given file 
         * in JSON format
         */
        inline void writeConfig(const std::string& filename)
        {
            std::ofstream file;
            file.open(filename);
            if (!file.is_open()) {
                std::runtime_error(
                    "Manager unable to write file: " 
                    + filename);
            }
            nlohmann::json j = saveJSON();
            file << j.dump(4);
            file.close();
        }

        /**
         * Import all Parameters and Devices configurarion
         * from given file in JSON format.
         * Throw std::runtime_error if given config
         * file is malformated
         */
        inline void readConfig(const std::string& filename)
        {
            std::ifstream file;
            file.open(filename);
            if (!file.is_open()) {
                std::runtime_error(
                    "Manager unable to read file: " 
                    + filename);
            }
            std::string config;
            std::string tmp;
            while(std::getline(file, tmp)) {
                config += tmp;
            }
            nlohmann::json j = nlohmann::json::parse(config); 
            loadJSON(j);
            file.close();
        }
};

}

