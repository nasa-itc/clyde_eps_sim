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

#ifndef ITC_EPS_BCR_HPP
#define ITC_EPS_BCR_HPP

#include "bus.hpp"
#include "adc.hpp"
#include <array>

namespace itc
{
    namespace eps
    {
        const int NUM_BCRS = 5; //!< Number of battery charge regulators (BCRs)

        /**
         * \brief Battery charge regulator (BCR) bus data
         */
        struct BcrData
        {
            BcrData() : voltage(), current(), temp(), sun{{ADC_CONV_THRESH}, {ADC_CONV_THRESH}} {}

            Channel voltage;    //!< Voltage
            Channel current[2]; //!< Current
            Channel temp[2];    //!< Temperature
            Channel sun[2];     //!< Sun sensor illumination
        };

        /**
         * \brief Battery charge regulator (BCR) bus
         */
        class BcrBus : public Bus
        {
        public:
            /**
             * \brief Constructor
             */
            BcrBus();

            /**
             * \brief Destructor
             */
            virtual ~BcrBus();

            /**
             * \brief Get battery charge regulator (BCR) bus data
             *
             * \param bcr Battery charge regulator (BCR) array number
             *
             * \return Battery charge regulator (BCR) bus data
             */
            BcrData* get_data(unsigned int bcr);

        private:
            /**
             * \brief On reset state change
             *
             * \param state Reset state
             */
            void on_reset(bool state);

        private:
            BcrData data[NUM_BCRS]; //!< Battery charge regulator (BCR) bus data
        };
    }
}

#endif

