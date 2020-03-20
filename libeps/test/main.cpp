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

#include "eps.hpp"
#include <ItcLogger/Logger.hpp>
#include <gtest/gtest.h>

using namespace itc::eps;

// default log level
const ItcLogger::log_level_t DEFAULT_LOG_LEVEL = ItcLogger::LOGGER_OFF;

int main(int argc, char **argv)
{
    // forward options to gtest
    ::testing::InitGoogleTest(&argc, argv);

    // create stdio logger
    ItcLogger::Logger *logger = ItcLogger::Logger::get(LOGGER_NAME.c_str());
    logger->set_level(DEFAULT_LOG_LEVEL);
    ItcLogger::TargetPtr target(new ItcLogger::Target(STDIO_BUILTIN_TARGET_IMPL, ItcLogger::LogArguments()));
    target->set_format("%t [%l] - %s");
    logger->add_target(target);

    // run tests
    logger->info("running eps tests");
    return RUN_ALL_TESTS();
}

