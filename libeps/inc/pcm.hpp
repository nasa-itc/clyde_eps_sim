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

#ifndef ITC_EPS_PCM_HPP
#define ITC_EPS_PCM_HPP

#include "bus.hpp"
#include "adc.hpp"

namespace itc
{
    namespace eps
    {
        /**
         * \brief Power conditioning module (PCM) bus type
         */
        enum PcmBusType
        {
            // note: enum ordering matters (used in command data bitfield)
            PCM_BUS_BAT = 0,
            PCM_BUS_5V,
            PCM_BUS_3V3,
            PCM_BUS_12V,
            NUM_PCM_BUSES
        };

        /**
         * \brief Power conditioning module (PCM) bus data
         */
        struct PcmData
        {
            PcmData() : voltage(), current() {}

            Channel voltage; //!< Voltage
            Channel current; //!< Current
        };

        /**
         * \brief Power conditioning module (PCM) bus
         */
        class PcmBus : public Bus
        {
        public:
            /**
             * \brief Constructor
             */
            PcmBus();

            /**
             * \brief Destructor
             */
            virtual ~PcmBus();

            /**
             * \brief Get power conditioning module (PCM) bus data
             *
             * \return Power conditioning module (PCM) bus data
             */
            PcmData* get_data();

        private:
            /**
             * \brief On reset state change
             *
             * \param state Reset state
             */
            void on_reset(bool state);

        private:
            PcmData data; //!< Power conditioning module (PCM) bus data
        };
    }
}

#endif

