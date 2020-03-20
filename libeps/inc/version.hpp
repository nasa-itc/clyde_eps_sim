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

#ifndef ITC_EPS_VERSION_HPP
#define ITC_EPS_VERSION_HPP

#include <cstdint>

namespace itc
{
    namespace eps
    {
        /**
         * \brief EPS board version
         */
        class Version
        {
        public:
            /**
             * \brief Constructor
             */
            Version() : version(0) {};

            /**
             * \brief Constructor
             *
             * \param version Raw board version
             */
            Version(uint16_t version) : version(version) {};

            /**
             * \brief Get firmware number
             *
             * \return Firmware number
             */
            uint16_t get_firmware() const
            {
                return version & 0xfff;
            }

            /**
             * \brief Get revision number
             *
             * \return Revision number
             */
            uint16_t get_revision() const
            {
                return (version >> 12) & 0xf;
            }

            /**
             * \brief Set board version
             *
             * \param firmware Firmware number
             * \param revision Revision number
             */
            void set_version(uint16_t firmware, uint16_t revision)
            {
                version = (firmware & 0xfff) | ((revision & 0xf) << 12);
            }

        public:
            uint16_t version; //!< Board version data
        };
    }
}

#endif

