#pragma once

#include <thread>
#include <functional>
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
            AggregateManager<Types...>(),
            _managerThread(nullptr),
            _managerThreadContinue(false)
        {
        }

        /**
         * End of pending Manager thread
         */
        inline ~Manager()
        {
            stopManagerThread();
        }
        
        /**
         * Export as json object all Parameters
         * and derived Devices configurations.
         */
        inline virtual nlohmann::json saveJSON() const override
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            nlohmann::json j = this->saveAggregatedJSON();
            j["Manager"] = this->_parametersList.saveJSON();
            j["Protocol"] = this->protocolParametersList().saveJSON();
            return j;
        }

        /**
         * Import from given json object all
         * Parameters and derived Devices configuration.
         * Throw std::runtime_error if 
         * given json is malformated.
         */
        inline virtual void loadJSON(const nlohmann::json& j) override
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            if (
                !j.is_object() ||
                j.size() > sizeof...(Types) + 1 ||
                j.count("Manager") != 1 ||
                j.count("Protocol") != 1
            ) {
                throw std::runtime_error(
                    "Manager load parameters root json malformed");
            }
            this->loadAggregatedJSON(j);
            this->_parametersList.loadJSON(j.at("Manager"));
            //Reset low level communication (bus/protocol)
            this->initBus();
            this->protocolParametersList().loadJSON(j.at("Protocol"));
        }

        /**
         * Export all Parameters and Devices 
         * configuration into given file 
         * in JSON format
         */
        inline virtual void writeConfig(
            const std::string& filename) const override
        {
            std::ofstream file;
            file.open(filename);
            if (!file.is_open()) {
                throw std::runtime_error(
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
        inline virtual void readConfig(
            const std::string& filename) override
        {
            std::ifstream file;
            file.open(filename);
            if (!file.is_open()) {
                throw std::runtime_error(
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

        /**
         * Start and stop the Manager thread
         * continiously calling flush() on 
         * manager instance. 
         * An optional callback function can 
         * be given and will be called at each
         * Manager cycle.
         */
        inline void startManagerThread(std::function<void()> callback = [](){})
        {
            if (_managerThread == nullptr) {
                _managerThreadContinue = true;
                _managerThread = new std::thread(
                    [this, callback](){
                        while (this->_managerThreadContinue) {
                            this->flush(false);
                            callback();
                        }
                    });
            }
        }
        inline void stopManagerThread()
        {
            if (_managerThread != nullptr) {
                _managerThreadContinue = false;
                _managerThread->join();
                delete _managerThread;
                _managerThread = nullptr;
            }
        }

    private:

        /**
         * Pointer to Manager thread 
         * instance and thread continue state
         */
        std::thread* _managerThread;
        bool _managerThreadContinue;
};

}

