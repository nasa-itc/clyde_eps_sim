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

#include "bcr.hpp"
#include "pcm.hpp"
#include "pdm.hpp"
#include <gtest/gtest.h>

using namespace itc::eps;

namespace
{
    class BusTest : public ::testing::Test
    {
    public:
        BusTest() :
            ::testing::Test(),
            bcr(),
            pcm(),
            pdm()
        {
            bcr.connect(pcm);
            pcm.connect(pdm);
            pdm.set_initial_state(true);
            pdm.set_state(true);
        }

    public:
        BcrBus bcr;
        PcmBus pcm;
        PdmBus pdm;
    };

    TEST_F(BusTest, Reset)
    {
        // set initial data
        bcr.get_data(0)->voltage.set_value(10);
        pcm.get_data()->voltage.set_value(15);
        pdm.get_data()->voltage.set_value(20);

        // test initial conditions
        EXPECT_DOUBLE_EQ(10, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(15, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(20, pdm.get_data()->voltage.get_value());
        EXPECT_FALSE(bcr.is_reset());
        EXPECT_FALSE(pcm.is_reset());
        EXPECT_FALSE(pdm.is_reset());

        // reset child pdm bus
        pdm.reset(true);
        EXPECT_FALSE(bcr.is_reset());
        EXPECT_FALSE(pcm.is_reset());
        EXPECT_TRUE(pdm.is_reset());
        pdm.get_data()->voltage.set_value(25);
        EXPECT_DOUBLE_EQ(10, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(15, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(0, pdm.get_data()->voltage.get_value());

        // release child pdm bus reset
        pdm.reset(false);
        EXPECT_FALSE(bcr.is_reset());
        EXPECT_FALSE(pcm.is_reset());
        EXPECT_FALSE(pdm.is_reset());
        EXPECT_DOUBLE_EQ(10, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(15, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(25, pdm.get_data()->voltage.get_value());
    }

    TEST_F(BusTest, ParentReset)
    {
        // set initial data
        bcr.get_data(0)->voltage.set_value(10);
        pcm.get_data()->voltage.set_value(15);
        pdm.get_data()->voltage.set_value(20);

        // test initial conditions
        EXPECT_DOUBLE_EQ(10, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(15, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(20, pdm.get_data()->voltage.get_value());
        EXPECT_FALSE(bcr.is_reset());
        EXPECT_FALSE(pcm.is_reset());
        EXPECT_FALSE(pdm.is_reset());

        // reset parent bcr bus
        bcr.reset(true);
        EXPECT_TRUE(bcr.is_reset());
        EXPECT_TRUE(pcm.is_reset());
        EXPECT_TRUE(pdm.is_reset());
        EXPECT_DOUBLE_EQ(0, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(0, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(0, pdm.get_data()->voltage.get_value());

        // release parent bcr bus reset
        bcr.reset(false);
        EXPECT_FALSE(bcr.is_reset());
        EXPECT_FALSE(pcm.is_reset());
        EXPECT_FALSE(pdm.is_reset());
        EXPECT_DOUBLE_EQ(10, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(15, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(20, pdm.get_data()->voltage.get_value());
    }

    TEST_F(BusTest, ReleaseChildWhileParentReset)
    {
        // set initial data
        bcr.get_data(0)->voltage.set_value(10);
        pcm.get_data()->voltage.set_value(15);
        pdm.get_data()->voltage.set_value(20);

        // test initial conditions
        EXPECT_DOUBLE_EQ(10, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(15, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(20, pdm.get_data()->voltage.get_value());
        EXPECT_FALSE(bcr.is_reset());
        EXPECT_FALSE(pcm.is_reset());
        EXPECT_FALSE(pdm.is_reset());

        // reset parent bcr bus
        bcr.reset(true);
        EXPECT_TRUE(bcr.is_reset());
        EXPECT_TRUE(pcm.is_reset());
        EXPECT_TRUE(pdm.is_reset());
        EXPECT_DOUBLE_EQ(0, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(0, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(0, pdm.get_data()->voltage.get_value());

        // release child pdm bus reset
        pdm.reset(false);
        EXPECT_TRUE(bcr.is_reset());
        EXPECT_TRUE(pcm.is_reset());
        EXPECT_TRUE(pdm.is_reset());
        EXPECT_DOUBLE_EQ(0, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(0, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(0, pdm.get_data()->voltage.get_value());

        // release parent bcr bus reset
        bcr.reset(false);
        EXPECT_FALSE(bcr.is_reset());
        EXPECT_FALSE(pcm.is_reset());
        EXPECT_FALSE(pdm.is_reset());
        EXPECT_DOUBLE_EQ(10, bcr.get_data(0)->voltage.get_value());
        EXPECT_DOUBLE_EQ(15, pcm.get_data()->voltage.get_value());
        EXPECT_DOUBLE_EQ(20, pdm.get_data()->voltage.get_value());
    }
}

