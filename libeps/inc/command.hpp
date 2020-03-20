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

#ifndef ITC_EPS_COMMAND_HPP
#define ITC_EPS_COMMAND_HPP

#include <cstdint>
#include <limits>
#include <map>

namespace itc
{
    namespace eps
    {
        const uint16_t CMD_RESP_ERROR = 0xffff; //!< Command response error

        /**
         * \brief EPS command type
         */
        enum CommandType
        {
            CMD_GET_BOARD_STATUS           = 0x01,
            CMD_GET_LAST_ERROR             = 0x03,
            CMD_GET_VERSION                = 0x04,
            CMD_GET_CHECKSUM               = 0x05,
            CMD_GET_TELEMETRY              = 0x10,
            CMD_GET_WDT_PERIOD             = 0x20,
            CMD_SET_WDT_PERIOD             = 0x21,
            CMD_RESET_WDT                  = 0x22,
            CMD_GET_NUM_BROWN_OUT_RESETS   = 0x31,
            CMD_GET_NUM_AUTO_SW_RESETS     = 0x32,
            CMD_GET_NUM_MANUAL_RESETS      = 0x33,
            CMD_GET_NUM_WDT_RESETS         = 0x34,
            CMD_SET_PDM_ALL_ON             = 0x40,
            CMD_SET_PDM_ALL_OFF            = 0x41,
            CMD_GET_PDM_ALL_ACTUAL_STATE   = 0x42,
            CMD_GET_PDM_ALL_EXPECTED_STATE = 0x43,
            CMD_GET_PDM_ALL_INITIAL_STATE  = 0x44,
            CMD_SET_PDM_ALL_INITIAL_STATE  = 0x45,
            CMD_SET_PDM_ON                 = 0x50,
            CMD_SET_PDM_OFF                = 0x51,
            CMD_SET_PDM_INITIAL_STATE_ON   = 0x52,
            CMD_SET_PDM_INITIAL_STATE_OFF  = 0x53,
            CMD_GET_PDM_ACTUAL_STATE       = 0x54,
            CMD_SET_PDM_TIMER_LIMIT        = 0x60,
            CMD_GET_PDM_TIMER_LIMIT        = 0x61,
            CMD_GET_PDM_TIMER_VALUE        = 0x62,
            CMD_SET_PCM_RESET              = 0x70,
            CMD_RESET_NODE                 = 0x80
        };

        /**
         * \brief Generic data range
         */
        template <typename T>
        struct DataRange
        {
            /**
             * \brief Constructor
             */
            DataRange() :
                min(std::numeric_limits<T>::min()),
                max(std::numeric_limits<T>::max())
            {}

            /**
             * \brief Constructor
             *
             * \param min Minimum value (inclusive)
             * \param max Maximum value (inclusive)
             */
            DataRange(T min, T max) :
                min(min), max(max)
            {}

            /**
             * \brief Check if value is within range (inclusive)
             *
             * \param val Value to check
             *
             * \return True if value is within range
             */
            template<typename U>
            bool is_valid(U val) const
            {
                return (val >= min) && (val <= max);
            }

            T min; //!< Minimum value (inclusive)
            T max; //!< Maximum value (inclusive)
        };

        /**
         * \brief EPS command data range type
         */
        typedef DataRange<uint8_t> CommandDataRange;

        /**
         * \brief EPS command data range map type
         */
        typedef std::map<CommandType, CommandDataRange> CommandDataRangeMap;

        /**
         * \brief EPS valid command data range map
         */
        const CommandDataRangeMap CMD_DATA_RANGES = {
            {CMD_SET_WDT_PERIOD, CommandDataRange(0x01, 0x5a)},
            {CMD_SET_PCM_RESET,  CommandDataRange(0x01, 0x0f)}
        };

        /**
         * \brief EPS valid channel data range map
         */
        const CommandDataRangeMap CMD_CHANNEL_RANGES = {
            {CMD_SET_PDM_ON,                CommandDataRange(0x01, 0x0a)},
            {CMD_SET_PDM_OFF,               CommandDataRange(0x01, 0x0a)},
            {CMD_SET_PDM_INITIAL_STATE_ON,  CommandDataRange(0x01, 0x0a)},
            {CMD_SET_PDM_INITIAL_STATE_OFF, CommandDataRange(0x01, 0x0a)},
            {CMD_GET_PDM_ACTUAL_STATE,      CommandDataRange(0x01, 0x0a)},
            {CMD_GET_PDM_TIMER_LIMIT,       CommandDataRange(0x01, 0x0a)},
            {CMD_GET_PDM_TIMER_VALUE,       CommandDataRange(0x01, 0x0a)}
        };
    }
}

#endif

