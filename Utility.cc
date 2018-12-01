#include <chrono>

#include "Utility.h"


float getSecondsSince(TimePoint startTime) {
    auto renderAndWriteTime = Clock::now() - startTime;
    return std::chrono::duration_cast<Milli>(renderAndWriteTime).count() / 1000.0f;
}
