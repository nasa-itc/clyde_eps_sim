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

#ifndef ITC_EPS_TYPES_HPP
#define ITC_EPS_TYPES_HPP

#include "version.hpp"
#include "status.hpp"
#include "adc.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace itc
{
    namespace eps
    {
        /**
         * \brief Byte swap config
         */
        struct ByteSwapConfig
        {
            ByteSwapConfig() : in(false), out(false) {}
            bool in;  //!< Swap incoming I2C data enable flag
            bool out; //!< Swap outgoing I2C data enable flag
        };

        const std::string LOGGER_NAME = "eps_sim"; //!< EPS sim logger name

        const int DEFAULT_WDT_TIMEOUT_MS = 4 * 60 * 1000; //!< Default watchdog timer (WDT) timeout (ms)
        const int DEFAULT_BUS_RESET_TIME_MS = 500;        //!< Default bus reset time (ms)
        
        typedef uint64_t SimTime; //!< Simulation time type

        typedef std::vector<uint8_t> I2CData; //!< I2C data type

        /**
         * \brief Channel telemetry
         */
        struct ChannelTelemetry
        {
            uint16_t digital; //!< Sampled digital telemetry
            double analog;    //!< Analog telemetry
        };

        typedef std::map<ChannelCode, ChannelTelemetry> Telemetry; //!< EPS channel telemetry map
    }
}

#endif

