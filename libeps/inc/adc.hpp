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

#ifndef ITC_EPS_ADC_HPP
#define ITC_EPS_ADC_HPP

#include <cstdint>
#include <vector>

namespace itc
{
    namespace eps
    {
        /**
         * \brief Analog telemetry channel code
         */
        enum ChannelCode
        {
            // bcr1
            CHANNEL_VBCR1    = 0xe110,
            CHANNEL_IBCR1A   = 0xe114,
            CHANNEL_IBCR1B   = 0xe115,
            CHANNEL_TBCR1A   = 0xe118,
            CHANNEL_TBCR1B   = 0xe119,
            CHANNEL_SDBCR1A  = 0xe11c,
            CHANNEL_SDBCR1B  = 0xe11d,
            // bcr2
            CHANNEL_VBCR2    = 0xe120,
            CHANNEL_IBCR2A   = 0xe124,
            CHANNEL_IBCR2B   = 0xe125,
            CHANNEL_TBCR2A   = 0xe128,
            CHANNEL_TBCR2B   = 0xe129,
            CHANNEL_SDBCR2A  = 0xe12c,
            CHANNEL_SDBCR2B  = 0xe12d,
            // bcr3
            CHANNEL_VBCR3    = 0xe130,
            CHANNEL_IBCR3A   = 0xe134,
            CHANNEL_IBCR3B   = 0xe135,
            CHANNEL_TBCR3A   = 0xe138,
            CHANNEL_TBCR3B   = 0xe139,
            CHANNEL_SDBCR3A  = 0xe13c,
            CHANNEL_SDBCR3B  = 0xe13d,
            // bcr4
            CHANNEL_VBCR4    = 0xe140,
            CHANNEL_IBCR4A   = 0xe144,
            CHANNEL_IBCR4B   = 0xe145,
            CHANNEL_TBCR4A   = 0xe148,
            CHANNEL_TBCR4B   = 0xe149,
            CHANNEL_SDBCR4A  = 0xe14c,
            CHANNEL_SDBCR4B  = 0xe14d,
            // bcr5
            CHANNEL_VBCR5    = 0xe150,
            CHANNEL_IBCR5A   = 0xe154,
            CHANNEL_IBCR5B   = 0xe155,
            CHANNEL_TBCR5A   = 0xe158,
            CHANNEL_TBCR5B   = 0xe159,
            CHANNEL_SDBCR5A  = 0xe15c,
            CHANNEL_SDBCR5B  = 0xe15d,
            // pcm
            CHANNEL_IPCM12V  = 0xe234,
            CHANNEL_VPCM12V  = 0xe230,
            CHANNEL_IPCMBATV = 0xe224,
            CHANNEL_VPCMBATV = 0xe220,
            CHANNEL_IPCM5V   = 0xe214,
            CHANNEL_VPCM5V   = 0xe210,
            CHANNEL_IPCM3V3  = 0xe204,
            CHANNEL_VPCM3V3  = 0xe200,
            // pdm
            CHANNEL_VSW1     = 0xe410,
            CHANNEL_ISW1     = 0xe414,
            CHANNEL_VSW2     = 0xe420,
            CHANNEL_ISW2     = 0xe424,
            CHANNEL_VSW3     = 0xe430,
            CHANNEL_ISW3     = 0xe434,
            CHANNEL_VSW4     = 0xe440,
            CHANNEL_ISW4     = 0xe444,
            CHANNEL_VSW5     = 0xe450,
            CHANNEL_ISW5     = 0xe454,
            CHANNEL_VSW6     = 0xe460,
            CHANNEL_ISW6     = 0xe464,
            CHANNEL_VSW7     = 0xe470,
            CHANNEL_ISW7     = 0xe474,
            CHANNEL_VSW8     = 0xe480,
            CHANNEL_ISW8     = 0xe484,
            CHANNEL_VSW9     = 0xe490,
            CHANNEL_ISW9     = 0xe494,
            CHANNEL_VSW10    = 0xe4a0,
            CHANNEL_ISW10    = 0xe4a4,
            // misc
            CHANNEL_IIDIODE  = 0xe284,
            CHANNEL_VIDIODE  = 0xe280,
            CHANNEL_I3V3_DRW = 0xe205,
            CHANNEL_I5V_DRW  = 0xe215,
            CHANNEL_TBRD     = 0xe308,
            CHANNEL_INVALID  = 0xffff
        };

        /**
         * \brief Analog channel converter type
         */
        enum ConverterType
        {
            ADC_CONV_LINEAR,
            ADC_CONV_THRESH
        };
        
        /**
         * \brief Analog channel converter parameters
         */
        typedef std::vector<double> ConverterParams;

        /**
         * \brief Analog telemetry channel
         */
        class Channel
        {
        public:
            /**
             * \brief Constructor
             *
             * Defaults to no conversion
             *
             * \param type Converter type
             * \param bits Channel resolution
             */
            Channel(ConverterType type = ADC_CONV_LINEAR, unsigned int bits = 10);

            /**
             * \brief Destructor
             */
            ~Channel();

            /**
             * \brief Configure channel converter
             *
             * \param params Converter parameters
             */
            void configure(const ConverterParams& params);

            /**
             * \brief Check if channel is active
             *
             * \return State of channel
             */
            bool is_active() const;

            /**
             * \brief Set active state of channel
             *
             * \param active State of channel
             */
            void set_active(bool active);

            /**
             * \brief Sample analog telemetry
             *
             * \return Sampled digital value (counts)
             */
            uint16_t sample() const;

            /**
             * \brief Get channel analog value
             *
             * \return Channel analog value
             */
            double get_value() const;

            /**
             * \brief Set channel analog value
             *
             * \param val Channel analog value
             */
            void set_value(double val);

        private:
            /**
             * \brief Get maximum digital value
             *
             * \return Maximum value based on resolution
             */
            unsigned int get_max_count() const;

        private:
            unsigned int resolution; //!< Channel resolution (bits)
            ConverterType type;      //!< Conversion type
            ConverterParams conv;    //!< Conversion parameters
            bool active;             //!< Flag indicating whether telemetry channel is active
            double value;            //!< Channel analog value
        };
    }
}

#endif

