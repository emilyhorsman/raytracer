#include <iomanip>
#include <iostream>
#include <string>

#include "Stats.h"


std::string quantityLabels[] = {
    "Primary Rays",
    "Shadow Rays",
    "Specular Rays",
    "Transmission Rays",
    "Intersections"
};


Stats::Stats()
: id(0)
, pixels(0)
, timeSeconds(0)
, quantities{ 0 }
{}


void Stats::print() {
    std::cout << "=== Render Thread "
              << std::right << std::setw(2) << std::setfill('0') << id
              << " ===" << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Time (seconds)" << timeSeconds << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Pixels" << pixels << std::endl;
    for (int i = 0; i < NUM_QUANTITIES; i++) {
        std::cout << std::left << std::setw(20) << std::setfill(' ') << quantityLabels[i] << quantities[i] << std::endl;
    }
    printf("\n");
}
