#include "CallManager.hpp"

namespace RhAL
{
CallManager::CallManager() : _paramScheduleMode("scheduleMode", true)
{
}

CallManager::~CallManager()
{
}

bool CallManager::isScheduleMode() const
{
  return _paramScheduleMode.value;
}

void CallManager::setScheduleMode(bool mode)
{
  std::lock_guard<std::mutex> lock(_mutex);
  _paramScheduleMode.value = mode;
}

}  // namespace RhAL
