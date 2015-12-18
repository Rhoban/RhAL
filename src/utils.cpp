#include <chrono>
#include "utils.h"

using namespace std::chrono;

double getTime()
{
    auto tp = steady_clock::now();
    return duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count() / 1000000000.0;
}

