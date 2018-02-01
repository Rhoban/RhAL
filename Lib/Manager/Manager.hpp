#pragma once

#include <thread>
#include <functional>
#include <json/json.h>
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
        inline virtual Json::Value saveJSON() const override
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            Json::Value j = this->saveAggregatedJSON();
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
        inline virtual void loadJSON(const Json::Value& j) override
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            if (
                !j.isObject() ||
                j.size() > sizeof...(Types) + 1 ||
                j["Manager"].isNull() ||
                j["Protocol"].isNull()
            ) {
                throw std::runtime_error(
                    "Manager load parameters root json malformed");
            }
            //Load Devices parameters
            this->loadAggregatedJSON(j);
            //Load Manager parameters (bus/protocol)
            this->_parametersList.loadJSON(j["Manager"]);
            //Reset low level communication (bus/protocol)
            this->initBus();
            //Load specific Protocol parameters
            this->protocolParametersList().loadJSON(j["Protocol"]);
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
            Json::Value j = saveJSON();
            Json::StyledWriter writer;
            file << writer.write(j);
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
            std::ifstream file(filename, std::ios::in | std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error(
                    "Manager unable to read file: " 
                    + filename);
            }
            std::string contents;
            file.seekg(0, std::ios::end);
            contents.resize(file.tellg());
            file.seekg(0, std::ios::beg);
            file.read(&contents[0], contents.size());
            file.close();
            // Create Json reader
            // TODO: investigate all the flags
            auto f=Json::Features::all();
            f.allowComments_=true;
            f.strictRoot_=false;
            f.allowDroppedNullPlaceholders_=true;
            f.allowNumericKeys_=true;
            Json::Reader reader(f);
            // Parse json
            // TODO: treat errors properly
            Json::Value j;
            reader.parse(contents, j);
            loadJSON(j);
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

