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
#include <map>

using namespace itc::eps;

namespace
{
    struct ChannelData
    {
        ChannelData(double conv0, double conv1, double value, ConverterType type = ADC_CONV_LINEAR) :
            type(type),
            conv{conv0, conv1},
            value(value)
        {}

        ConverterType type;
        double conv[2];
        double value;
    };

    typedef std::map<ChannelCode, ChannelData> ChannelDataMap;

    const ChannelDataMap CHANNEL_DATA = {
        // bcr1
        {CHANNEL_VBCR1,    ChannelData(0.0249,     0.0,      20.25)},
        {CHANNEL_IBCR1A,   ChannelData(0.0009775,  0.0,      2.25)},
        {CHANNEL_IBCR1B,   ChannelData(0.0009775,  0.0,      2.75)},
        {CHANNEL_TBCR1A,   ChannelData(0.4963,     -273.15,  5.0)},
        {CHANNEL_TBCR1B,   ChannelData(0.4963,     -273.15,  10.0)},
        {CHANNEL_SDBCR1A,  ChannelData(512,        0.0,      100, ADC_CONV_THRESH)},
        {CHANNEL_SDBCR1B,  ChannelData(512,        0.0,      775, ADC_CONV_THRESH)},
        // bcr2
        {CHANNEL_VBCR2,    ChannelData(0.0249,     0.0,      19.50)},
        {CHANNEL_IBCR2A,   ChannelData(0.0009775,  0.0,      1.75)},
        {CHANNEL_IBCR2B,   ChannelData(0.0009775,  0.0,      2.00)},
        {CHANNEL_TBCR2A,   ChannelData(0.4963,     -273.15,  7.5)},
        {CHANNEL_TBCR2B,   ChannelData(0.4963,     -273.15,  15.0)},
        {CHANNEL_SDBCR2A,  ChannelData(512,        0.0,      600, ADC_CONV_THRESH)},
        {CHANNEL_SDBCR2B,  ChannelData(512,        0.0,      300, ADC_CONV_THRESH)},
        // bcr3
        {CHANNEL_VBCR3,    ChannelData(0.00997,    0.0,      6.00)},
        {CHANNEL_IBCR3A,   ChannelData(0.0009775,  0.0,      0.25)},
        {CHANNEL_IBCR3B,   ChannelData(0.0009775,  0.0,      0.75)},
        {CHANNEL_TBCR3A,   ChannelData(0.4963,     -273.15,  20.5)},
        {CHANNEL_TBCR3B,   ChannelData(0.4963,     -273.15,  21.75)},
        {CHANNEL_SDBCR3A,  ChannelData(512,        0.0,      511, ADC_CONV_THRESH)},
        {CHANNEL_SDBCR3B,  ChannelData(512,        0.0,      512, ADC_CONV_THRESH)},
        // pcm
        {CHANNEL_IPCM12V,  ChannelData(0.00207,   0.0,  1.2)},
        {CHANNEL_VPCM12V,  ChannelData(0.01349,   0.0,  12.3)},
        {CHANNEL_IPCMBATV, ChannelData(0.005237,  0.0,  4.2)},
        {CHANNEL_VPCMBATV, ChannelData(0.009971,  0.0,  7.7)},
        {CHANNEL_IPCM5V,   ChannelData(0.005237,  0.0,  4.1)},
        {CHANNEL_VPCM5V,   ChannelData(0.005865,  0.0,  5.1)},
        {CHANNEL_IPCM3V3,  ChannelData(0.005237,  0.0,  3.9)},
        {CHANNEL_VPCM3V3,  ChannelData(0.003988,  0.0,  3.3)},
        // pdm
        {CHANNEL_VSW1,     ChannelData(0.01349,   0.0,  11.9)},
        {CHANNEL_ISW1,     ChannelData(0.001328,  0.0,  1.25)},
        {CHANNEL_VSW2,     ChannelData(0.01349,   0.0,  12.1)},
        {CHANNEL_ISW2,     ChannelData(0.001328,  0.0,  1.50)},
        {CHANNEL_VSW3,     ChannelData(0.009971,  0.0,  7.7)},
        {CHANNEL_ISW3,     ChannelData(0.001328,  0.0,  1.75)},
        {CHANNEL_VSW4,     ChannelData(0.009971,  0.0,  8.1)},
        {CHANNEL_ISW4,     ChannelData(0.001328,  0.0,  2.0)},
        {CHANNEL_VSW5,     ChannelData(0.005865,  0.0,  4.9)},
        {CHANNEL_ISW5,     ChannelData(0.001328,  0.0,  2.25)},
        {CHANNEL_VSW6,     ChannelData(0.005865,  0.0,  5.0)},
        {CHANNEL_ISW6,     ChannelData(0.001328,  0.0,  2.50)},
        {CHANNEL_VSW7,     ChannelData(0.005865,  0.0,  5.1)},
        {CHANNEL_ISW7,     ChannelData(0.001328,  0.0,  2.75)},
        {CHANNEL_VSW8,     ChannelData(0.004311,  0.0,  3.1)},
        {CHANNEL_ISW8,     ChannelData(0.001328,  0.0,  3.00)},
        {CHANNEL_VSW9,     ChannelData(0.004311,  0.0,  3.2)},
        {CHANNEL_ISW9,     ChannelData(0.001328,  0.0,  3.25)},
        {CHANNEL_VSW10,    ChannelData(0.004311,  0.0,  3.3)},
        {CHANNEL_ISW10,    ChannelData(0.001328,  0.0,  3.50)}
    };
}

