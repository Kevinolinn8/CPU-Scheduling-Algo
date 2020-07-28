#include "burst.hpp"

#include <cassert>
#include <stdexcept>

Burst::Burst(BurstType type, int length)
{
    this->burst_type = type;
    this->length = length;
}

void Burst::update_time(int delta_t)
{
   // Burst::length -= delta_t;
}
