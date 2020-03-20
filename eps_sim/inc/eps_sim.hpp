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

#ifndef ITC_EPS_SIM_HPP
#define ITC_EPS_SIM_HPP

#include "eps.hpp"
#include "config.hpp"
#include <Common/types.hpp>
#include <I2C/Client/I2CSlave.hpp>
#include <mutex>

namespace NosEngine
{
    namespace Client
    {
        class Bus;
        class DataNode;
    }
}

class EpsWindow;
class Fl_Widget;

namespace itc
{
    namespace eps
    {
        /**
         * \brief EPS simulator
         */
        class EpsSim : public NosEngine::I2C::I2CSlave
        {
        public:
            /**
             * \brief Constructor
             *
             * \param config EPS simulator config
             */
            EpsSim(const Config& config);

            /**
             * \brief Destructor
             */
            virtual ~EpsSim();

            /**
             * \brief Minimize the EPS simulator window
             */
            void minimize();

            /*
             * \brief I2C master read
             *
             * \param rbuf Read data buffer
             * \param rlen Read data buffer length
             *
             * \return Number of bytes read
             */
            size_t i2c_read(uint8_t* rbuf, size_t rlen);

            /*
             * \brief I2C master write
             *
             * \param wbuf Write data buffer
             * \param wlen Write data buffer length
             *
             * \return Number of bytes written
             */
            size_t i2c_write(const uint8_t *wbuf, size_t wlen);

        private:
            /**
             * \brief Callback to handle NOS time ticks
             *
             * \param time New NOS time
             */
            void on_time_tick(NosEngine::Common::SimTime time);

            /**
             * \brief Callback to handle telemetry updates
             *
             * \param widget Telemetry widget
             * \param user User data
             */
            static void on_tlm_update(Fl_Widget *widget, void *user);

            /**
             * \brief Callback to handle switch state updates
             *
             * \param widget Switch widget
             * \param user User data
             */
            static void on_switch_update(Fl_Widget *widget, void *user);

            /**
             * \brief Update window with latest EPS data
             */
            void update_win();

        private:
            std::mutex mutex;   //!< Mutex for thread safety

            NosEngine::Client::Bus time_bus; //!< NOS client time bus
            unsigned int tick_ms; //!< NOS time tick (ms)

            itc::eps::Eps eps; //!< EPS simulator

            EpsWindow *win; //!< EPS simulator window
        };
    }
}

#endif

