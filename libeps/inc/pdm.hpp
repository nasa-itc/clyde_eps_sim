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

#ifndef ITC_EPS_PDM_HPP
#define ITC_EPS_PDM_HPP

#include "bus.hpp"
#include "adc.hpp"
#include "types.hpp"
#include <cstdint>

namespace itc
{
    namespace eps
    {
        const int NUM_SWITCHES = 10; //!< Number of power distribution module (PDM) switches

        /**
         * \brief Power distribution module (PDM) bus data
         */
        struct PdmData
        {
            PdmData() : voltage(), current() {}

            Channel voltage;     //!< Voltage
            Channel current;     //!< Current
        };

        /**
         * \brief Power distribution module (PDM) bus
         */
        class PdmBus : public Bus
        {
        public:
            /**
             * \brief Constructor
             */
            PdmBus();

            /**
             * \brief Destructor
             */
            ~PdmBus();

            /**
             * \brief Set current simulation time
             *
             * \param time Simulation time (ms)
             */
            void set_time(SimTime time);

            /**
             * \brief Get initial power on reset (POR) switch state
             *
             * \return Initial switch POR state
             */
            bool get_initial_state() const;

            /**
             * \brief Set initial power on reset (POR) switch state
             *
             * \param state Initial switch POR state
             */
            void set_initial_state(bool state);

            /**
             * \brief Get switch state
             *
             * \return Switch state
             */
            bool get_state() const;

            /**
             * \brief Set switch state
             *
             * \param state Switch state
             */
            void set_state(bool state);

            /**
             * \brief Get switch timer limit
             *
             * \return Switch timer limit
             */
            uint8_t get_timer_limit() const;

            /**
             * \brief Set switch timer limit
             *
             * \param limit Switch timer limit
             */
            void set_timer_limit(uint8_t limit);

            /**
             * \brief Get current switch timer value
             *
             * \return Current switch timer value
             */
            uint8_t get_timer_value() const;

            /**
             * \brief Get power distribution module (PDM) bus data
             *
             * \return Power distribution module (PDM) bus data
             */
            PdmData* get_data();

        private:
            /**
             * \brief Get state of switch
             *
             * \return True if switch is permanently disabled
             */
            bool is_switch_disabled() const;

            /**
             * \brief Get state of switch timer
             *
             * \return True if timer is currently active
             */
            bool is_timer_active() const;

            /**
             * \brief Get switch auto-off time (ms)
             *
             * \return Switch auto-off time (ms)
             */
            SimTime get_off_time() const;

            /**
             * \brief On reset state change
             *
             * \param state Reset state
             */
            void on_reset(bool state);

        private:
            bool initial_state;  //!< Initial power on reset (POR) switch state
            bool state;          //!< Switch state

            uint8_t timer_limit; //!< Switch timer limit
            SimTime time_ms;     //!< Current simulation time (ms)
            SimTime start_ms;    //!< Switch timer start time (ms)

            PdmData data; //!< Power distribution module (PDM) bus data
        };
    }
}

#endif

