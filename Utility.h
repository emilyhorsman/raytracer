#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <chrono>


typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> TimePoint;
typedef std::chrono::milliseconds Milli;


float getSecondsSince(TimePoint startTime);


#endif
