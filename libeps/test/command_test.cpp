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

#include "common.hpp"
#include "eps.hpp"
#include "command.hpp"
#include "types.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include <string>

using namespace itc::eps;

namespace
{
    const uint8_t I2C_ADDRESS = 20;
    const uint8_t CMD_INVALID = 0xff;
    const uint8_t PDM_INVALID = 0xff;

    class CommandTest : public ::testing::Test
    {
    public:
        CommandTest() :
            ::testing::Test(),
            eps(I2C_ADDRESS, false)
        {}

        // simple i2c write/read transaction
        I2CData send_command(CommandType type, uint8_t param)
        {
            I2CData data{type, param};
            eps.i2c_write(data);
            eps.i2c_read(data);
            return data;
        }

        // simple i2c write/read transaction (2 byte param)
        I2CData send_tlm_command(CommandType type, uint16_t param)
        {
            uint8_t high = (param >> 8) & 0xff;
            uint8_t low = param & 0xff;
            I2CData data{type, high, low};
            eps.i2c_write(data);
            eps.i2c_read(data);
            return data;
        }

        // pack switch number and timer limit and send i2c command
        I2CData send_timer_limit_command(uint8_t num, uint8_t limit)
        {
            I2CData data{CMD_SET_PDM_TIMER_LIMIT, num, limit};
            eps.i2c_write(data);
            eps.i2c_read(data);
            return data;
        }

        // test command status flags
        void test_command_status()
        {
            Status status = eps.get_status();
            EXPECT_FALSE(status.is_set(STATUS_INVALID_CMD));
            EXPECT_FALSE(status.is_set(STATUS_INVALID_DATA));
            EXPECT_FALSE(status.is_set(STATUS_INVALID_CHANNEL));
        }

        // unpack response data to 2 or 4 byte word
        uint32_t unpack_response(const I2CData& data)
        {
            uint32_t resp = 0;
            if(data.size() == 2)
            {
                resp = (data[0] << 8) | data[1];
            }
            else if(data.size() == 4)
            {
                resp = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
            }
            return resp;
        }

        Eps eps;
    };

    TEST_F(CommandTest, GetBoardStatus)
    {
        I2CData data;

        data = send_command(CMD_GET_BOARD_STATUS, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());

        Status status;
        status.set(STATUS_INVALID_CMD, true);
        status.set(RESET_WDT);
        eps.set_status(status);

        data = send_command(CMD_GET_BOARD_STATUS, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());

        status = Status(unpack_response(data));
        EXPECT_TRUE(status.is_set(STATUS_INVALID_CMD));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_DATA));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CHANNEL));
        EXPECT_TRUE(status.is_set(RESET_WDT));
    }

    TEST_F(CommandTest, GetLastError)
    {
        I2CData data;
        uint16_t resp;

        data = send_command(CMD_GET_LAST_ERROR, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        resp = unpack_response(data);
        EXPECT_EQ(0, resp);

        data = send_command(static_cast<CommandType>(CMD_INVALID), 0);
        EXPECT_EQ(2, data.size());
        resp = unpack_response(data);
        EXPECT_EQ(CMD_RESP_ERROR, resp);

        data = send_command(CMD_GET_LAST_ERROR, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        resp = unpack_response(data);
        EXPECT_EQ(ERROR_INVALID_CMD, resp);

        data = send_command(CMD_SET_PDM_ON, PDM_INVALID);
        EXPECT_EQ(2, data.size());
        resp = unpack_response(data);
        EXPECT_EQ(CMD_RESP_ERROR, resp);

        data = send_command(CMD_GET_LAST_ERROR, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        resp = unpack_response(data);
        EXPECT_EQ(ERROR_INVALID_CHANNEL, resp);
    }

    TEST_F(CommandTest, GetVersion)
    {
        I2CData data;
        uint16_t resp;

        data = send_command(CMD_GET_VERSION, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        EXPECT_EQ(0, unpack_response(data));

        Version version;
        version.set_version(0xbcd, 0xa);
        eps.set_version(version);

        data = send_command(CMD_GET_VERSION, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        resp = unpack_response(data);
        EXPECT_EQ(0xabcd, resp);

        version = Version(resp);
        EXPECT_EQ(0xa, version.get_revision());
        EXPECT_EQ(0xbcd, version.get_firmware());
    }

    TEST_F(CommandTest, GetChecksum)
    {
        I2CData data;

        data = send_command(CMD_GET_CHECKSUM, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        EXPECT_EQ(0xdead, unpack_response(data));
    }

    TEST_F(CommandTest, GetTelemetry)
    {
        I2CData data;

        // set telemetry
        for(ChannelDataMap::const_iterator it = CHANNEL_DATA.begin(); it != CHANNEL_DATA.end(); ++it)
        {
            eps.set_telemetry(it->first, it->second.value);
        }

        // test all channels
        for(ChannelDataMap::const_iterator it = CHANNEL_DATA.begin(); it != CHANNEL_DATA.end(); ++it)
        {
            ChannelCode code = it->first;

            // configure channel
            ConverterParams params;
            params.push_back(it->second.conv[0]);
            params.push_back(it->second.conv[1]);

            eps.configure_channel(code, params);

            Channel channel(it->second.type);
            channel.set_value(it->second.value);
            channel.configure(params);
            EXPECT_DOUBLE_EQ(it->second.value, channel.get_value());

            data = send_tlm_command(CMD_GET_TELEMETRY, static_cast<uint16_t>(code));
            test_command_status();
            EXPECT_EQ(2, data.size());
            EXPECT_EQ(channel.sample(), unpack_response(data));
        }
    }

    TEST_F(CommandTest, GetSetWatchdogTimeout)
    {
        I2CData data;
        Status status;

        // trigger wdt reset
        eps.set_time(DEFAULT_WDT_TIMEOUT_MS);
        status = eps.get_status();
        EXPECT_TRUE(eps.is_reset());
        EXPECT_TRUE(status.is_set(RESET_WDT));
        EXPECT_EQ(1, status.get_num_resets(RESET_WDT));
        status.clear(RESET_WDT);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        
        // wait for reset
        eps.set_time(eps.get_time() + DEFAULT_BUS_RESET_TIME_MS);
        EXPECT_FALSE(eps.is_reset());

        // change wdt timeout
        data = send_command(CMD_SET_WDT_PERIOD, 10);
        test_command_status();
        EXPECT_EQ(0, data.size());

        // wait 1ms prior to reset
        eps.set_time(eps.get_time() + (10*60*1000) - 1);
        EXPECT_FALSE(eps.is_reset());

        // verify wdt reset
        eps.set_time(eps.get_time() + 1);
        status = eps.get_status();
        EXPECT_TRUE(eps.is_reset());
        EXPECT_TRUE(status.is_set(RESET_WDT));
        EXPECT_EQ(2, status.get_num_resets(RESET_WDT));
        status.clear_resets();
        EXPECT_FALSE(status.is_set(RESET_WDT));

        // get wdt timeout (while in reset)
        data = send_command(CMD_GET_WDT_PERIOD, 0);
        test_command_status();
        EXPECT_EQ(0, data.size());

        // wait for reset
        eps.set_time(eps.get_time() + DEFAULT_BUS_RESET_TIME_MS);
        EXPECT_FALSE(eps.is_reset());

        // get wdt timeout
        data = send_command(CMD_GET_WDT_PERIOD, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        EXPECT_EQ(10, unpack_response(data));
    }

    TEST_F(CommandTest, SetWatchdogTimeout)
    {
        I2CData data;
        Status status;

        // iterate through all possible values (1-90 minutes)
        for(int i = 1; i <= 90; i++)
        {
            // change wdt timeout
            data = send_command(CMD_SET_WDT_PERIOD, i);
            test_command_status();
            EXPECT_EQ(0, data.size());

            // get wdt timeout
            data = send_command(CMD_GET_WDT_PERIOD, 0);
            test_command_status();
            EXPECT_EQ(2, data.size());
            EXPECT_EQ(i, unpack_response(data));
        }

        // iterate through all invalid values
        for(int i = 91; i <= 0xff; i++)
        {
            // change wdt timeout
            data = send_command(CMD_SET_WDT_PERIOD, i);
            status = eps.get_status();
            EXPECT_FALSE(status.is_set(STATUS_INVALID_CMD));
            EXPECT_TRUE(status.is_set(STATUS_INVALID_DATA));
            EXPECT_FALSE(status.is_set(STATUS_INVALID_CHANNEL));
            EXPECT_EQ(2, data.size());
            EXPECT_EQ(CMD_RESP_ERROR, unpack_response(data));

            // get wdt timeout
            data = send_command(CMD_GET_WDT_PERIOD, 0);
            test_command_status();
            EXPECT_EQ(2, data.size());
            EXPECT_EQ(90, unpack_response(data));
        }
    }

    TEST_F(CommandTest, ResetWatchdog)
    {
        I2CData data;
        Status status;

        // wait 1ms prior to reset
        eps.set_time(DEFAULT_WDT_TIMEOUT_MS - 1);
        status = eps.get_status();
        EXPECT_FALSE(eps.is_reset());
        EXPECT_FALSE(status.is_set(RESET_WDT));

        // reset wdt
        data = send_command(CMD_RESET_WDT, 0);
        test_command_status();
        EXPECT_EQ(0, data.size());
        status = eps.get_status();
        EXPECT_FALSE(eps.is_reset());
        EXPECT_FALSE(status.is_set(RESET_WDT));

        // wait for next reset cycle to verify wdt timer was reset
        eps.set_time(eps.get_time() + DEFAULT_WDT_TIMEOUT_MS - 1);
        status = eps.get_status();
        EXPECT_FALSE(eps.is_reset());
        EXPECT_FALSE(status.is_set(RESET_WDT));
    }

    // brown-out / auto resets not modeled
    TEST_F(CommandTest, GetResetCounts)
    {
        I2CData data;
        Status status;

        for(int i = 0; i < 10; i++)
        {
            data = send_command(CMD_GET_NUM_WDT_RESETS, 0);
            test_command_status();
            EXPECT_EQ(2, data.size());
            EXPECT_EQ(i, unpack_response(data));

            status = eps.get_status();
            EXPECT_EQ(i, status.get_num_resets(RESET_WDT));

            // force and wait for watchdog timer reset
            eps.set_time(eps.get_time() + DEFAULT_WDT_TIMEOUT_MS);
            EXPECT_TRUE(eps.is_reset());
            eps.set_time(eps.get_time() + DEFAULT_BUS_RESET_TIME_MS);
            EXPECT_FALSE(eps.is_reset());
        }
        
        // release reset
        eps.set_time(eps.get_time() + DEFAULT_BUS_RESET_TIME_MS);
        EXPECT_FALSE(eps.is_reset());

        for(int i = 0; i < 10; i++)
        {
            data = send_command(CMD_GET_NUM_MANUAL_RESETS, 0);
            test_command_status();
            EXPECT_EQ(2, data.size());
            EXPECT_EQ(i, unpack_response(data));

            status = eps.get_status();
            EXPECT_EQ(i, status.get_num_resets(RESET_MANUAL));

            // reset node and wait
            data = send_command(CMD_RESET_NODE, 0);
            test_command_status();
            EXPECT_EQ(0, data.size());
            EXPECT_TRUE(eps.is_reset());
            eps.set_time(eps.get_time() + DEFAULT_BUS_RESET_TIME_MS);
            EXPECT_FALSE(eps.is_reset());
        }
    }

    TEST_F(CommandTest, GetSetPdmAllState)
    {
        I2CData data;
        ChannelCode codes[NUM_SWITCHES][2] = {{CHANNEL_VSW1,  CHANNEL_ISW1},
                                              {CHANNEL_VSW2,  CHANNEL_ISW2},
                                              {CHANNEL_VSW3,  CHANNEL_ISW3},
                                              {CHANNEL_VSW4,  CHANNEL_ISW4},
                                              {CHANNEL_VSW5,  CHANNEL_ISW5},
                                              {CHANNEL_VSW6,  CHANNEL_ISW6},
                                              {CHANNEL_VSW7,  CHANNEL_ISW7},
                                              {CHANNEL_VSW8,  CHANNEL_ISW8},
                                              {CHANNEL_VSW9,  CHANNEL_ISW9},
                                              {CHANNEL_VSW10, CHANNEL_ISW10}};

        // set telemetry
        for(ChannelDataMap::const_iterator it = CHANNEL_DATA.begin(); it != CHANNEL_DATA.end(); ++it)
        {
            eps.set_telemetry(it->first, it->second.value);
        }

        // toggle on
        data = send_command(CMD_SET_PDM_ALL_ON, 0);
        test_command_status();
        EXPECT_EQ(0, data.size());
        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            EXPECT_TRUE(eps.get_switch_state(i));

            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                ChannelDataMap::const_iterator it = CHANNEL_DATA.find(codes[i][j]);
                EXPECT_DOUBLE_EQ(it->second.value, tlm.analog);
            }
        }

        data = send_command(CMD_GET_PDM_ALL_ACTUAL_STATE, 0);
        test_command_status();
        EXPECT_EQ(4, data.size());
        EXPECT_EQ(0x3ff << 1, unpack_response(data));

        // toggle off
        data = send_command(CMD_SET_PDM_ALL_OFF, 0);
        test_command_status();
        EXPECT_EQ(0, data.size());
        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            EXPECT_FALSE(eps.get_switch_state(i));

            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                EXPECT_DOUBLE_EQ(0, tlm.analog);
            }
        }

        data = send_command(CMD_GET_PDM_ALL_ACTUAL_STATE, 0);
        test_command_status();
        EXPECT_EQ(4, data.size());
        EXPECT_EQ(0, unpack_response(data));

        // toggle on
        data = send_command(CMD_SET_PDM_ALL_ON, 0);
        test_command_status();
        EXPECT_EQ(0, data.size());
        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            EXPECT_TRUE(eps.get_switch_state(i));

            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                ChannelDataMap::const_iterator it = CHANNEL_DATA.find(codes[i][j]);
                EXPECT_DOUBLE_EQ(it->second.value, tlm.analog);
            }
        }

        data = send_command(CMD_GET_PDM_ALL_ACTUAL_STATE, 0);
        test_command_status();
        EXPECT_EQ(4, data.size());
        EXPECT_EQ(0x3ff << 1, unpack_response(data));
    }

    /*
    TEST_F(CommandTest, GetSetPdmAllInitialState)
    {
        // TODO how is this command sent?? docs are not clear
        I2CData data;
        Telemetry tlm;

        // initial state on
        data = send_command(CMD_SET_PDM_ALL_INITIAL_STATE, 0x3ff << 1);
        test_command_status();
        EXPECT_EQ(0, data.size());
        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            EXPECT_TRUE(tlm.pdm[i].initial_state);
        }

        data = send_command(CMD_GET_PDM_ALL_INITIAL_STATE, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        EXPECT_EQ(0x3ff << 1, unpack_response(data));

        // initial state off
        data = send_command(CMD_SET_PDM_ALL_INITIAL_STATE, 0);
        test_command_status();
        EXPECT_EQ(0, data.size());
        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            EXPECT_FALSE(tlm.pdm[i].initial_state);
        }

        data = send_command(CMD_GET_PDM_ALL_INITIAL_STATE, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        EXPECT_EQ(0, unpack_response(data));
        
        // initial state on
        data = send_command(CMD_SET_PDM_ALL_INITIAL_STATE, 0x3ff << 1);
        test_command_status();
        EXPECT_EQ(0, data.size());
        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            EXPECT_TRUE(tlm.pdm[i].initial_state);
        }

        data = send_command(CMD_GET_PDM_ALL_INITIAL_STATE, 0);
        test_command_status();
        EXPECT_EQ(2, data.size());
        EXPECT_EQ(0x3ff << 1, unpack_response(data));
    }
    */

    TEST_F(CommandTest, GetSetPdmState)
    {
        I2CData data;
        ChannelCode codes[NUM_SWITCHES][2] = {{CHANNEL_VSW1,  CHANNEL_ISW1},
                                              {CHANNEL_VSW2,  CHANNEL_ISW2},
                                              {CHANNEL_VSW3,  CHANNEL_ISW3},
                                              {CHANNEL_VSW4,  CHANNEL_ISW4},
                                              {CHANNEL_VSW5,  CHANNEL_ISW5},
                                              {CHANNEL_VSW6,  CHANNEL_ISW6},
                                              {CHANNEL_VSW7,  CHANNEL_ISW7},
                                              {CHANNEL_VSW8,  CHANNEL_ISW8},
                                              {CHANNEL_VSW9,  CHANNEL_ISW9},
                                              {CHANNEL_VSW10, CHANNEL_ISW10}};

        // set telemetry
        for(ChannelDataMap::const_iterator it = CHANNEL_DATA.begin(); it != CHANNEL_DATA.end(); ++it)
        {
            eps.set_telemetry(it->first, it->second.value);
        }

        // toggle off
        data = send_command(CMD_SET_PDM_ALL_OFF, 0);
        test_command_status();
        EXPECT_EQ(0, data.size());

        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            // verify switch off
            EXPECT_FALSE(eps.get_switch_state(i));
            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                EXPECT_DOUBLE_EQ(0, tlm.analog);
            }

            // turn switch on
            data = send_command(CMD_SET_PDM_ON, i+1);
            test_command_status();
            EXPECT_EQ(0, data.size());

            data = send_command(CMD_GET_PDM_ACTUAL_STATE, i+1);
            test_command_status();
            EXPECT_EQ(2, data.size());
            EXPECT_EQ(1, unpack_response(data));

            EXPECT_TRUE(eps.get_switch_state(i));
            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                ChannelDataMap::const_iterator it = CHANNEL_DATA.find(codes[i][j]);
                EXPECT_DOUBLE_EQ(it->second.value, tlm.analog);
            }

            // turn switch off
            data = send_command(CMD_SET_PDM_OFF, i+1);
            test_command_status();
            EXPECT_EQ(0, data.size());

            data = send_command(CMD_GET_PDM_ACTUAL_STATE, i+1);
            test_command_status();
            EXPECT_EQ(2, data.size());
            EXPECT_EQ(0, unpack_response(data));

            EXPECT_FALSE(eps.get_switch_state(i));
            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                EXPECT_DOUBLE_EQ(0, tlm.analog);
            }
        }
    }

    TEST_F(CommandTest, GetSetPdmInitialState)
    {
        I2CData data;
        CommandType type;

        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            // get initial state
            bool state = eps.get_switch_initial_state(i);

            // toggle switch state
            type = state ? CMD_SET_PDM_INITIAL_STATE_OFF : CMD_SET_PDM_INITIAL_STATE_ON;
            data = send_command(type, i+1);
            test_command_status();
            EXPECT_EQ(0, data.size());
            
            // test state change
            EXPECT_EQ(!state, eps.get_switch_initial_state(i));
            
            // toggle switch state
            state = !state;
            type = state ? CMD_SET_PDM_INITIAL_STATE_OFF : CMD_SET_PDM_INITIAL_STATE_ON;
            data = send_command(type, i+1);
            test_command_status();
            EXPECT_EQ(0, data.size());
            
            // test state change
            EXPECT_EQ(!state, eps.get_switch_initial_state(i));
        }
    }

    TEST_F(CommandTest, GetSetPdmTimerLimit)
    {
        I2CData data;
        CommandType type;

        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            // set initial switch state
            eps.set_switch_state(i, true);
            
            for(int j = 0; j <= 0xff; j++)
            {
                // set timer limit
                data = send_timer_limit_command(i+1, j);
                test_command_status();
                EXPECT_EQ(0, data.size());
                
                // get timer limit
                data = send_command(CMD_GET_PDM_TIMER_LIMIT, i+1);
                test_command_status();
                EXPECT_EQ(2, data.size());
            
                // test response
                EXPECT_EQ(j, unpack_response(data));
            }
        }
    }

    TEST_F(CommandTest, GetPdmTimerValue)
    {
        I2CData data;
        CommandType type;
        SimTime time_ms = 0;
        SimTime start_ms = 0;

        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            // set initial switch state
            eps.set_switch_state(i, true);
            EXPECT_TRUE(eps.get_switch_state(i));

            // set timer limit (120s)
            data = send_timer_limit_command(i+1, 0x4);
            test_command_status();
            EXPECT_EQ(0, data.size());

            // timer start time
            start_ms = time_ms;

            // test auto off time
            for(int j = 0; j < 120; j++)
            {
                time_ms += 1000;
                eps.set_time(time_ms);

                // get timer limit
                data = send_command(CMD_GET_PDM_TIMER_VALUE, i+1);
                test_command_status();
                EXPECT_EQ(2, data.size());
                
                // test response
                uint8_t exp_time_val = static_cast<uint8_t>(((time_ms - start_ms) / 1000.0) / 30.0);
                EXPECT_EQ(exp_time_val, unpack_response(data));

                // test switch state
                EXPECT_EQ(j != 119, eps.get_switch_state(i));
            }
        }
    }

    TEST_F(CommandTest, SetPdmTimerDisabled)
    {
        I2CData data;
        CommandType type;

        for(int i = 0; i < NUM_SWITCHES; i++)
        {
            // set initial switch state
            eps.set_switch_state(i, true);
            EXPECT_TRUE(eps.get_switch_state(i));

            // set timer limit (disabled)
            data = send_timer_limit_command(i+1, 0x0);
            test_command_status();
            EXPECT_EQ(0, data.size());

            // test switch state
            EXPECT_FALSE(eps.get_switch_state(i));

            // test switch disabled
            eps.set_switch_state(i, true);
            EXPECT_FALSE(eps.get_switch_state(i));
        }
    }

    TEST_F(CommandTest, SetPcmReset)
    {
        I2CData data;
        uint8_t param = 0;
        ChannelCode codes[NUM_PCM_BUSES][2] = {{CHANNEL_VPCMBATV, CHANNEL_IPCMBATV},
                                               {CHANNEL_VPCM5V,   CHANNEL_IPCM5V},
                                               {CHANNEL_VPCM3V3,  CHANNEL_IPCM3V3},
                                               {CHANNEL_VPCM12V,  CHANNEL_IPCM12V}};

        // set telemetry
        for(ChannelDataMap::const_iterator it = CHANNEL_DATA.begin(); it != CHANNEL_DATA.end(); ++it)
        {
            eps.set_telemetry(it->first, it->second.value);
        }

        for(int i = 0; i < NUM_PCM_BUSES; i++)
        {
            // reset time
            eps.set_time(0);

            // test initial telemetry
            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                ChannelDataMap::const_iterator it = CHANNEL_DATA.find(codes[i][j]);
                EXPECT_DOUBLE_EQ(it->second.value, tlm.analog);
            }

            // reset bus
            param = 1 << i;
            data = send_command(CMD_SET_PCM_RESET, param);
            test_command_status();
            EXPECT_EQ(0, data.size());

            // test reset telemetry
            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                EXPECT_DOUBLE_EQ(0.0, tlm.analog);
            }

            // test just prior to reset release
            eps.set_time(DEFAULT_BUS_RESET_TIME_MS - 1);
            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                EXPECT_DOUBLE_EQ(0.0, tlm.analog);
            }

            // test reset release
            eps.set_time(DEFAULT_BUS_RESET_TIME_MS + 1);
            for(int j = 0; j < 2; j++)
            {
                ChannelTelemetry tlm;
                eps.get_telemetry(codes[i][j], tlm);
                ChannelDataMap::const_iterator it = CHANNEL_DATA.find(codes[i][j]);
                EXPECT_DOUBLE_EQ(it->second.value, tlm.analog);
            }
        }
    }
}

