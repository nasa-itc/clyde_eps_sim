/* Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
   limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
   for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
   any warranty that the software will be error free.


   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
   arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,

   contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
   documentation or services provided hereunder

   ITC Team
   NASA IV&V
   ivv-itc@lists.nasa.gov
*/

#include "adc.hpp"
#include <limits>

using namespace itc::eps;

Channel::Channel(ConverterType type, unsigned int bits) :
    resolution(bits),
    type(type),
    conv{1, 0},
    active(true),
    value(0)
{
}

Channel::~Channel()
{
}

void Channel::configure(const ConverterParams& params)
{
    conv = params;
    if(conv.size() < 2)
    {
        conv.resize(2, 0);
    }
}

bool Channel::is_active() const
{
    return active;
}

void Channel::set_active(bool active)
{
    this->active = active;
}

uint16_t Channel::sample() const
{
    uint16_t sample = 0;
    if(active)
    {
        switch(type)
        {
            case ADC_CONV_LINEAR:
            {
                double den = conv[0] + std::numeric_limits<double>::min(); // ensure non-zero value
                double count = ((value - conv[1]) / den) + 0.5;
                if(count >= 0)
                {
                    sample = static_cast<uint16_t>(count) & get_max_count();
                }
                break;
            }
            case ADC_CONV_THRESH:
                sample = static_cast<uint16_t>(value);
                break;
            default:
                sample = 0;
                break;
        }
    }
    return sample;
}

double Channel::get_value() const
{
    return active ? value : 0.0;
}

void Channel::set_value(double val)
{
    value = val;
}

unsigned int Channel::get_max_count() const
{
    return (1 << resolution) - 1;
}

