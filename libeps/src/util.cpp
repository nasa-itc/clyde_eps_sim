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

#include "util.hpp"

using namespace itc::eps;

std::string itc::eps::to_string(CommandType type)
{
    switch(type)
    {
        case CMD_GET_BOARD_STATUS:
            return "GET_BOARD_STATUS";
        case CMD_GET_LAST_ERROR:
            return "GET_LAST_ERROR";
        case CMD_GET_VERSION:
            return "GET_VERSION";
        case CMD_GET_CHECKSUM:
            return "GET_CHECKSUM";
        case CMD_GET_TELEMETRY:
            return "GET_TELEMETRY";
        case CMD_GET_WDT_PERIOD:
            return "GET_WATCHDOG_PERIOD";
        case CMD_SET_WDT_PERIOD:
            return "SET_WATCHDOG_PERIOD";
        case CMD_RESET_WDT:
            return "RESET_WATCHDOG";
        case CMD_GET_NUM_BROWN_OUT_RESETS:
            return "GET_NUM_BROWN_OUT_RESETS";
        case CMD_GET_NUM_AUTO_SW_RESETS:
            return "GET_NUM_AUTO_SOFTWARE_RESETS";
        case CMD_GET_NUM_MANUAL_RESETS:
            return "GET_NUM_MANUAL_RESETS";
        case CMD_GET_NUM_WDT_RESETS:
            return "GET_NUM_WATCHDOG_RESETS";
        case CMD_SET_PDM_ALL_ON:
            return "SET_PDM_ALL_ON";
        case CMD_SET_PDM_ALL_OFF:
            return "SET_PDM_ALL_OFF";
        case CMD_GET_PDM_ALL_ACTUAL_STATE:
            return "GET_PDM_ALL_ACTUAL_STATE";
        case CMD_GET_PDM_ALL_EXPECTED_STATE:
            return "GET_PDM_ALL_EXPECTED_STATE";
        case CMD_GET_PDM_ALL_INITIAL_STATE:
            return "GET_PDM_ALL_INITIAL_STATE";
        case CMD_SET_PDM_ALL_INITIAL_STATE:
            return "SET_PDM_ALL_INITIAL_STATE";
        case CMD_SET_PDM_ON:
            return "SET_PDM_ON";
        case CMD_SET_PDM_OFF:
            return "SET_PDM_OFF";
        case CMD_SET_PDM_INITIAL_STATE_ON:
            return "SET_PDM_INITIAL_STATE_ON";
        case CMD_SET_PDM_INITIAL_STATE_OFF:
            return "SET_PDM_INITIAL_STATE_OFF";
        case CMD_GET_PDM_ACTUAL_STATE:
            return "GET_PDM_ACTUAL_STATE";
        case CMD_SET_PDM_TIMER_LIMIT:
            return "SET_PDM_TIMER_LIMIT";
        case CMD_GET_PDM_TIMER_LIMIT:
            return "GET_PDM_TIMER_LIMIT";
        case CMD_GET_PDM_TIMER_VALUE:
            return "GET_PDM_TIMER_VALUE";
        case CMD_SET_PCM_RESET:
            return "SET_PCM_RESET";
        case CMD_RESET_NODE:
            return "RESET_NODE";
        default:
            return "UNKNOWN";
    }
}

std::string itc::eps::to_string(PcmBusType type)
{
    switch(type)
    {
        case PCM_BUS_BAT:
            return "PCM_BUS_BATTERY";
        case PCM_BUS_12V:
            return "PCM_BUS_12V";
        case PCM_BUS_5V:
            return "PCM_BUS_5V";
        case PCM_BUS_3V3:
            return "PCM_BUS_3.3V";
        default:
            return "UNKNOWN";
    }
}

std::string itc::eps::to_string(ErrorCode code)
{
    switch(code)
    {
        case ERROR_NONE:
            return "NONE";
        case ERROR_INVALID_CMD:
            return "INVALID_CMD";
        case ERROR_INVALID_DATA:
            return "INVALID_DATA";
        case ERROR_INVALID_CHANNEL:
            return "INVALID_CHANNEL";
        case ERROR_INACTIVE_CHANNEL:
            return "INACTIVE_CHANNEL";
        case ERROR_INVALID_CRC:
            return "INVALID_CRC";
        case ERROR_RESET:
            return "RESET";
        case ERROR_ADC:
            return "ADC";
        case ERROR_EEPROM_READ:
            return "EEPROM_READ";
        case ERROR_INTERNAL_SPI:
            return "INTERNAL_SPI";
        default:
            return "UNKNOWN";
    }
}

