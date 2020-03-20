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

#include "status.hpp"
#include <gtest/gtest.h>

using namespace itc::eps;

namespace
{
    TEST(StatusTest, CommandStatus)
    {
        Status status;

        EXPECT_FALSE(status.is_set(STATUS_INVALID_CMD));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_DATA));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CHANNEL));
        EXPECT_EQ(0, status.get_status());

        status.set(STATUS_INVALID_CMD, true);
        EXPECT_TRUE(status.is_set(STATUS_INVALID_CMD));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_DATA));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CHANNEL));
        EXPECT_EQ(1 << 0, status.get_status());

        status.set(STATUS_INVALID_CMD, false);
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CMD));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_DATA));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CHANNEL));
        EXPECT_EQ(0, status.get_status());

        status.set(STATUS_INVALID_DATA, true);
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CMD));
        EXPECT_TRUE(status.is_set(STATUS_INVALID_DATA));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CHANNEL));
        EXPECT_EQ(1 << 2, status.get_status());

        status.set(STATUS_INVALID_DATA, false);
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CMD));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_DATA));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CHANNEL));
        EXPECT_EQ(0, status.get_status());

        status.set(STATUS_INVALID_CHANNEL, true);
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CMD));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_DATA));
        EXPECT_TRUE(status.is_set(STATUS_INVALID_CHANNEL));
        EXPECT_EQ(1 << 3, status.get_status());

        status.set(STATUS_INVALID_CHANNEL, false);
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CMD));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_DATA));
        EXPECT_FALSE(status.is_set(STATUS_INVALID_CHANNEL));
        EXPECT_EQ(0, status.get_status());
    }

    TEST(StatusTest, EEPROM)
    {
        Status status;

        EXPECT_FALSE(status.is_set(STATUS_EEPROM_ERROR));
        EXPECT_EQ(0, status.get_status());

        status.set(STATUS_EEPROM_ERROR, true);
        EXPECT_TRUE(status.is_set(STATUS_EEPROM_ERROR));
        EXPECT_EQ(1 << 4, status.get_status());

        status.set(STATUS_EEPROM_ERROR, false);
        EXPECT_FALSE(status.is_set(STATUS_EEPROM_ERROR));
        EXPECT_EQ(0, status.get_status());
    }

    TEST(StatusTest, ResetType)
    {
        Status status;

        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(0, status.get_status());

        status.set(RESET_WDT);
        EXPECT_TRUE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(1 << 1, status.get_status());

        status.clear(RESET_WDT);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(0, status.get_status());

        status.set(RESET_POWER_ON);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_TRUE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(1 << 5, status.get_status());

        status.clear(RESET_POWER_ON);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(0, status.get_status());

        status.set(RESET_BROWN_OUT);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_TRUE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(1 << 6, status.get_status());

        status.clear(RESET_BROWN_OUT);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(0, status.get_status());

        status.set(RESET_MANUAL);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_TRUE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(0, status.get_status());

        status.clear(RESET_MANUAL);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(0, status.get_status());
    }

    TEST(StatusTest, MutualExclusiveReset)
    {
        Status status;

        status.set(RESET_WDT);
        EXPECT_TRUE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(1 << 1, status.get_status());

        status.set(RESET_POWER_ON);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_TRUE(status.is_set(RESET_POWER_ON));
        EXPECT_FALSE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(1 << 5, status.get_status());

        status.set(RESET_BROWN_OUT);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_TRUE(status.is_set(RESET_BROWN_OUT));
        EXPECT_FALSE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(1 << 6, status.get_status());

        status.set(RESET_MANUAL);
        EXPECT_FALSE(status.is_set(RESET_WDT));
        EXPECT_FALSE(status.is_set(RESET_POWER_ON));
        EXPECT_TRUE(status.is_set(RESET_BROWN_OUT));
        EXPECT_TRUE(status.is_set(RESET_MANUAL));
        EXPECT_EQ(1 << 6, status.get_status());
    }

    TEST(StatusTest, ResetCount)
    {
        Status status;

        for(int i = 0; i < 5; i++)
        {
            status.set(RESET_WDT);
        }
        EXPECT_EQ(5, status.get_num_resets(RESET_WDT));
        EXPECT_EQ(0, status.get_num_resets(RESET_POWER_ON));
        EXPECT_EQ(0, status.get_num_resets(RESET_BROWN_OUT));
        EXPECT_EQ(0, status.get_num_resets(RESET_MANUAL));
        for(int i = 0; i < 5; i++)
        {
            status.set(RESET_POWER_ON);
        }
        EXPECT_EQ(5, status.get_num_resets(RESET_WDT));
        EXPECT_EQ(5, status.get_num_resets(RESET_POWER_ON));
        EXPECT_EQ(0, status.get_num_resets(RESET_BROWN_OUT));
        EXPECT_EQ(0, status.get_num_resets(RESET_MANUAL));
        for(int i = 0; i < 5; i++)
        {
            status.set(RESET_BROWN_OUT);
        }
        EXPECT_EQ(5, status.get_num_resets(RESET_WDT));
        EXPECT_EQ(5, status.get_num_resets(RESET_POWER_ON));
        EXPECT_EQ(5, status.get_num_resets(RESET_BROWN_OUT));
        EXPECT_EQ(0, status.get_num_resets(RESET_MANUAL));
        for(int i = 0; i < 5; i++)
        {
            status.set(RESET_MANUAL);
        }
        EXPECT_EQ(5, status.get_num_resets(RESET_WDT));
        EXPECT_EQ(5, status.get_num_resets(RESET_POWER_ON));
        EXPECT_EQ(5, status.get_num_resets(RESET_BROWN_OUT));
        EXPECT_EQ(5, status.get_num_resets(RESET_MANUAL));
    }

    TEST(StatusTest, ErrorCode)
    {
        Status status;

        EXPECT_EQ(ERROR_NONE, status.get_last_error());
        status.set(STATUS_INVALID_CMD, true);
        EXPECT_EQ(ERROR_INVALID_CMD, status.get_last_error());
        status.set(STATUS_INVALID_DATA, true);
        EXPECT_EQ(ERROR_INVALID_DATA, status.get_last_error());
        status.set(STATUS_INVALID_CHANNEL, true);
        EXPECT_EQ(ERROR_INVALID_CHANNEL, status.get_last_error());
        status.set(STATUS_EEPROM_ERROR, true);
        EXPECT_EQ(ERROR_EEPROM_READ, status.get_last_error());
        status.set(RESET_WDT);
        EXPECT_EQ(ERROR_RESET, status.get_last_error());
    }
}

