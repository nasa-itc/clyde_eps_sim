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

/*
 * References:
 *  - User Manual: 3rd Generation EPS (XUA)
 *      Document No.: USM-01-01317
 *      Revision: E
 *      Date: 12/01/2016
 *  
 *  - Supplemental User Manual for 01-02668 - 3G EPS XUA No Inhibits with 2x12V, 4x5V, 4x3V3 PDMs
 *      Document No.: USM-01-02668
 *      Revision: A
 *      Date: 07/01/2016
 */

#ifndef ITC_EPS_HPP
#define ITC_EPS_HPP

#include "types.hpp"
#include "version.hpp"
#include "status.hpp"
#include "adc.hpp"
#include "bcr.hpp"
#include "pcm.hpp"
#include "pdm.hpp"
#include <algorithm>
#include <cstdint>
#include <set>
#include <map>

namespace itc
{
    namespace eps
    {
        /**
         * \brief Clyde Space 3rd generation electrical power system (EPS) simulator
         */
        class Eps
        {
        public:
            /**
             * \brief Constructor
             *
             * \param address I2C base address
             * \param daughterboard Flag indicating whether daughterboard is connected
             * \param swap_config Byte swap config for incoming/outgoing I2C data
             */
            Eps(uint8_t address, bool daughterboard, ByteSwapConfig swap_config = ByteSwapConfig());

            /**
             * \brief Destructor
             */
            ~Eps();

            /**
             * \brief I2C master write
             *
             * \param data I2C write data (command and data)
             */
            void i2c_write(const I2CData& data);
            
            /**
             * \brief I2C master read
             *
             * \param data I2C read data (command response)
             */
            void i2c_read(I2CData& data);

            /**
             * \brief Get I2C address
             *
             * \return I2C address
             */
            uint8_t get_address() const;

            /**
             * \brief Get current simulation time
             *
             * \return Simulation time (ms)
             */
            SimTime get_time() const;

            /**
             * \brief Set current simulation time
             *
             * \param time Simulation time (ms)
             */
            void set_time(SimTime time);

            /**
             * \brief Get EPS board version
             *
             * \return EPS board version
             */
            Version get_version() const;

            /**
             * \brief Set EPS board version
             *
             * \param version EPS board version
             */
            void set_version(const Version& version);

            /**
             * \brief Get EPS board status
             *
             * \return EPS board status
             */
            Status get_status() const;

            /**
             * \brief Set EPS board status
             *
             * \param status EPS board status
             */
            void set_status(const Status& status);

            /**
             * \brief Get EPS daughterboard version
             *
             * \return EPS daughterboard version
             */
            Version get_daughterboard_version() const;

            /**
             * \brief Set EPS daughterboard version
             *
             * \param version EPS daughterboard version
             */
            void set_daughterboard_version(const Version& version);

            /**
             * \brief Get EPS daughterboard status
             *
             * \return EPS daughterboard status
             */
            Status get_daughterboard_status() const;

            /**
             * \brief Set EPS daughterboard status
             *
             * \param status EPS daughterboard status
             */
            void set_daughterboard_status(const Status& status);

            /**
             * \brief Get all telemetry
             *
             * \param tlm All telemetry
             */
            void get_telemetry(Telemetry& tlm) const;

            /**
             * \brief Get telemetry
             *
             * \param code Analog channel telemetry code
             * \param tlm Analog channel telemetry
             */
            void get_telemetry(ChannelCode code, ChannelTelemetry& tlm) const;

            /**
             * \brief Set default telemetry value
             *
             * \param code Analog channel telemetry code
             * \param val Analog telemetry value
             */
            void set_telemetry(ChannelCode code, double val);

            /**
             * \brief Get power distribution module (PDM) switch state
             *
             * \param num PDM switch number
             *
             * \return Switch state
             */
            bool get_switch_state(unsigned int num) const;

            /**
             * \brief Set power distribution module (PDM) switch state
             *
             * \param num PDM switch number
             * \param on Switch state
             */
            void set_switch_state(unsigned int num, bool on);

            /**
             * \brief Get power distribution module (PDM) switch initial power on reset (POR) state
             *
             * \param num PDM switch number
             *
             * \return Switch initial power on reset (POR) state
             */
            bool get_switch_initial_state(unsigned int num) const;

            /**
             * \brief Set power distribution module (PDM) switch initial power on reset (POR) state
             *
             * \param num PDM switch number
             * \param on Switch initial power on reset (POR) state
             */
            void set_switch_initial_state(unsigned int num, bool on);

            /**
             * \brief Get EPS reset state
             *
             * \return True if EPS is in reset state
             */
            bool is_reset() const;

            /**
             * \brief Configure analog telemetry channel
             *
             * \param code Analog channel telemetry code
             * \param params Converter parameters
             */
            void configure_channel(ChannelCode code, const ConverterParams& params);

        private:
            /**
             * \brief Get validity of telemetry channel
             *
             * \return True if telemetry channel is valid
             */
            bool is_channel_valid(uint16_t code) const;

            /**
             * \brief Read telemetry
             *
             * \param param Telemetry channel
             *
             * \return Telemetry channel value (counts)
             */
            uint16_t read_telemetry(uint16_t param);

            /**
             * \brief Set power condition module (PCM) reset
             *
             * \param data Command data
             */
            void set_pcm_reset(uint8_t data);

            /**
             * \brief Set state of all power distribution module (PDM) switches
             *
             * \param initial If true set initial state, otherwise set current state
             * \param state PDM switch state
             */
            void set_pdm_all(bool initial, bool state);

            /**
             * \brief Get state of all power distribution module (PDM) switches
             *
             * \param initial If true get initial state, otherwise get current state
             *
             * \return PDM switch state
             */
            uint32_t get_pdm_all(bool initial) const;

            /**
             * \brief Set power distribution module (PDM) switch auto off timer limit
             *
             * \param data PDM switch timer limit data
             */
            void set_pdm_timer_limit(uint16_t data);

            /**
             * \brief Get state of all power conditioning module (PCM) buses
             *
             * \return PCM bus state
             */
            uint16_t get_pcm_state() const;

            /**
             * \brief Reset bus
             *
             * \param bus Bus to reset
             */
            void reset_bus(Bus& bus);

            /**
             * \brief Connect analog telemetry channels
             */
            void connect_channels();

            /**
             * \brief Connect power buses
             */
            void connect_buses();

            /**
             * \brief Get parameter from I2C command data
             *
             * \param data I2C command data
             *
             * \return Command parameter
             */
            uint32_t get_command_param(const I2CData& data);

            /**
             * \brief Set I2C command response
             *
             * \param data Command response data
             */
            template<typename T>
            void set_response(const T& data);

        private:
            /**
             * \brief Bus reset info
             *
             * Simple comparator to determine if reset time has elapsed
             */
            struct ResetInfo
            {
                ResetInfo(Bus* bus, SimTime time_ms) : bus(bus), time_ms(time_ms) {}
                bool operator<(const ResetInfo& rhs) const {return time_ms < rhs.time_ms;}
                Bus *bus;
                SimTime time_ms;
            };

            ByteSwapConfig swap; //!< Byte swap config for incoming/outgoing I2C data

            uint8_t address;  //!< I2C address
            I2CData response; //!< I2C response data

            SimTime time_ms; //!< Current simulation time (ms)

            Version version; //!< EPS board version
            Status status;   //!< EPS board status
            BcrBus bcr_bus;  //!< Battery charge regulator (BCR) bus
            PcmBus pcm_bus[NUM_PCM_BUSES]; //!< Power conditioning module (PCM) buses
            PdmBus pdm_bus[NUM_SWITCHES];  //!< Power distribution module (PDM) switch buses
            bool db_connected;  //!< Flag indicating whether daughterboard is connected
            Version db_version; //!< EPS daughterboard version
            Status db_status;   //!< EPS daughterboard status

            typedef std::map<ChannelCode, Channel> MiscChannels; //!< EPS board misc channel map type
            MiscChannels misc_channels; //!< Misc board telemetry channels

            typedef std::map<ChannelCode, Channel*> Converter; //!< Analog to digital converter type
            Converter adc; //!< Analog digital converter

            unsigned int wdt_time_ms;    //!< Watchdog timer time (ms)
            unsigned int wdt_timeout_ms; //!< Watchdog timer timeout value (ms)
            
            typedef std::set<ResetInfo> ResetBusSet; //!< Reset bus set
            ResetBusSet reset_buses; //!< Buses in reset state
        };

        template<typename T>
        void Eps::set_response(const T& data)
        {
            response.clear();
            const uint8_t *raw = reinterpret_cast<const uint8_t*>(&data);
            response.insert(response.begin(), raw, raw + sizeof(data));

            // swap byte order
            if(swap.out)
            {
                std::reverse(response.begin(), response.end());
            }
        }
    }
}

#endif

