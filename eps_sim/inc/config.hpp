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

#ifndef ITC_EPS_CONFIG_HPP
#define ITC_EPS_CONFIG_HPP

#include "adc.hpp"
#include "version.hpp"
#include "types.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace itc
{
    namespace eps
    {
        /**
         * \brief NOS engine config
         */
        struct NosConfig
        {
            std::string uri;      //!< NOS server URI
            std::string node;     //!< NOS node name
            std::string i2c_bus;  //!< NOS I2C hardware bus name
            std::string time_bus; //!< NOS time bus name
            unsigned int tick_ms; //!< NOS time tick (ms)
        };

        /**
         * \brief EPS simulator config
         */
        class Config
        {
        public:
            /**
             * \brief Constructor
             *
             * \param cfgfile The EPS simulator config file to load
             *
             * \throw boost::property_tree::json_parser_error Error parsing config file
             */
            Config(const std::string& cfgfile = "");

            /**
             * \brief Load the EPS simulator config file
             *
             * \param cfgfile The EPS simulator config file to load
             *
             * \throw boost::property_tree::json_parser_error Error parsing config file
             */
            void load(const std::string& cfgfile);
            
        public:
            std::string log_level; //!< Log level
            ByteSwapConfig swap;   //!< byte swap config
            NosConfig nos;         //!< NOS engine config
            uint8_t eps_address;   //!< EPS I2C base address

            Version version; //!< EPS board version

            bool db_connected;  //!< EPS daughterboard connected
            Version db_version; //!< EPS daughterboard version

            typedef std::vector<bool> SwitchStates;
            SwitchStates switch_states; //!< Power distribution module (PDM) switch states

            typedef std::map<ChannelCode, double> Telemetry;
            Telemetry tlm; //!< Default sim telemetry

            typedef std::map<ChannelCode, ConverterParams> ChannelConfig;
            ChannelConfig adc; //!< Analog telemetry channel config

        private:
            /**
             * \brief Get array values from config
             *
             * \param cfg Config section containing array
             * \param size Expected array size
             *
             * \return Array values
             */
            template <typename T>
            std::vector<T> get_config_array(boost::property_tree::ptree& cfg, unsigned int size)
            {
                std::vector<T> values;
                BOOST_FOREACH(boost::property_tree::ptree::value_type &val, cfg)
                {
                    values.push_back(val.second.get_value<T>());
                }
                values.resize(size, T());
                return values;
            }
        };
    }
}

#endif

