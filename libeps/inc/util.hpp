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

#ifndef ITC_EPS_UTIL_HPP
#define ITC_EPS_UTIL_HPP

#include "command.hpp"
#include "pcm.hpp"
#include "status.hpp"
#include <string>
#include <sstream>

namespace itc
{
    namespace eps
    {
        /**
         * \brief Convert value to string
         *
         * \param value Value to convert
         * \param hex If true convert to hex string
         *
         * \return Value converted to string
         */
        template<typename T>
        std::string to_string(const T& value, bool hex = false)
        {
            std::string result;
            std::stringstream ss;
            if(hex) ss << std::hex;
            ss << value;
            ss >> result;
            return result;
        }

        /**
         * \brief Convert string to value
         *
         * \param src Source string to convert
         * \param hex Flag indicating source is hex string
         *
         * \return Value of string
         */
        template<typename T>
        T from_string(const std::string& src, bool hex = false)
        {
            T result;
            std::stringstream ss;
            if(hex) ss << std::hex;
            ss << src;
            ss >> result;
            return result;
        }

        /**
         * \brief Convert command type enum to string
         *
         * \param type Command type
         *
         * \return String representation of command type enum
         */
        std::string to_string(CommandType type);

        /**
         * \brief Convert power conditioning module (PCM) bus type enum to string
         *
         * \param type PCM bus type
         *
         * \return String representation of PCM bus type enum
         */
        std::string to_string(PcmBusType type);

        /**
         * \brief Convert EPS board error code enum to string
         *
         * \param code Error code
         *
         * \return String representation of error code enum
         */
        std::string to_string(ErrorCode code);
    }
}

#endif

