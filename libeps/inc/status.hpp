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

#ifndef ITC_EPS_STATUS_HPP
#define ITC_EPS_STATUS_HPP

#include <cstdint>
#include <bitset>
#include <map>

namespace itc
{
    namespace eps
    {
        const uint16_t CHECKSUM = 0xdead; //!< Default checksum value

        /**
         * \brief Board status bits
         */
        enum BoardStatus
        {
            STATUS_INVALID_CMD     = 0,
            STATUS_RESET_WDT       = 1,
            STATUS_INVALID_DATA    = 2,
            STATUS_INVALID_CHANNEL = 3,
            STATUS_EEPROM_ERROR    = 4,
            STATUS_RESET_POWER_ON  = 5,
            STATUS_RESET_BROWN_OUT = 6,
            NUM_STATUS_BITS        = 7
        };

        /**
         * \brief Board reset type
         */
        enum ResetType
        {
            RESET_WDT       = 1,
            RESET_POWER_ON  = 5,
            RESET_BROWN_OUT = 6,
            RESET_MANUAL    = 0xff
        };

        /**
         * \brief Board error codes
         */
        enum ErrorCode
        {
            ERROR_NONE             = 0x00,
            ERROR_INVALID_CMD      = 0x01,
            ERROR_INVALID_DATA     = 0x02,
            ERROR_INVALID_CHANNEL  = 0x03,
            ERROR_INACTIVE_CHANNEL = 0x04,
            ERROR_INVALID_CRC      = 0x10,
            ERROR_RESET            = 0x13,
            ERROR_ADC              = 0x14,
            ERROR_EEPROM_READ      = 0x20,
            ERROR_INTERNAL_SPI     = 0x30
        };

        /**
         * \brief EPS board status
         */
        class Status
        {
        public:
            /**
             * \brief Constructor
             */
            Status();

            /**
             * \brief Constructor
             *
             * \param status Board status data
             */
            Status(uint16_t status);

            /**
             * \brief Destructor
             */
            ~Status();

            /**
             * \brief Get board checksum
             *
             * \return Board checksum
             */
            uint16_t get_checksum() const;

            /**
             * \brief Get raw board status data
             *
             * \return Board status data
             */
            uint16_t get_status() const;

            /**
             * \brief Set raw board status data
             *
             * \param status Board status data
             */
            void set_status(uint16_t status);

            /**
             * \brief Get state of board status bit
             *
             * \param bit Board status bit
             *
             * \return True if status bit set
             */
            bool is_set(BoardStatus bit) const;

            /**
             * \brief Set board status bit
             *
             * \param bit Board status bit
             * \param state Bit state
             */
            void set(BoardStatus bit, bool state);

            /**
             * \brief Get reset type state
             *
             * \param type Reset type
             *
             * \return True if reset type is set
             */
            bool is_set(ResetType type) const;

            /**
             * \brief Set reset type
             *
             * \param type Reset type
             */
            void set(ResetType type);

            /**
             * \brief Clear all board status bits
             */
            void clear_all();

            /**
             * \brief Clear reset type
             */
            void clear(ResetType type);

            /**
             * \brief Clear all resets
             */
            void clear_resets();

            /**
             * \brief Get total number of system resets
             *
             * \param type Reset type
             *
             * \return Total number of system resets
             */
            uint8_t get_num_resets(ResetType type) const;

            /**
             * \brief Get last error code
             *
             * \return Last error code
             */
            ErrorCode get_last_error() const;

        private:
            typedef std::map<ResetType, uint8_t> ResetCount;

            uint16_t checksum;       //!< Checksum of board ROM
            std::bitset<16> status;  //!< Board status bits
            bool manual_reset;       //!< Flag indicating manual reset active
            ResetCount reset_counts; //!< Reset counters per type
            ErrorCode last_error;    //!< Last error code
        };
    }
}

#endif

