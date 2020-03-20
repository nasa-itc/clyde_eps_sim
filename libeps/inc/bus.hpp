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

#ifndef ITC_EPS_BUS_HPP
#define ITC_EPS_BUS_HPP

#include <string>
#include <set>

namespace itc
{
    namespace eps
    {
        /**
         * \brief Abstract resettable EPS power bus
         */
        class Bus
        {
        public:
            /**
             * \brief Constructor
             *
             * \param name Bus name
             */
            Bus(const std::string& name = "");

            /**
             * \brief Destructor
             */
            virtual ~Bus();

            /**
             * \brief Set bus name
             *
             * \param name Bus name
             */
            void set_name(const std::string& name);

            /**
             * \brief Get bus name
             *
             * \return Bus name
             */
            std::string get_name() const;

            /**
             * \brief Connect child bus as reset target
             *
             * \param bus Downstream child bus
             */
            void connect(Bus& bus);

            /**
             * \brief Get bus reset state
             *
             * \return True if bus is in reset state
             */
            bool is_reset() const;

            /**
             * \brief Set bus reset state
             *
             * \param state Reset state
             */
            void reset(bool state);

            /**
             * \brief On reset state change
             *
             * \param state Reset state
             */
            virtual void on_reset(bool state) = 0;

        private:
            /**
             * \brief Add parent bus as reset source
             *
             * \param bus Parent bus
             */
            void add_parent(Bus& bus);

        private:
            typedef std::set<Bus*> BusSet; //!< Bus set

            std::string name;    //!< Bus name
            BusSet parent_buses; //!< Parent buses (reset sources)
            BusSet child_buses;  //!< Child buses (reset sinks)
            bool reset_state;    //!< Bus reset state
        };
    }
}

#endif

