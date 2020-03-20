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

#ifndef ITC_EPS_WIDGETS_HPP
#define ITC_EPS_WIDGETS_HPP

#include "adc.hpp"
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Light_Button.H>
#include <map>

namespace itc
{
    namespace eps
    {
        /**
         * \brief Telemetry input widget
         *
         * This widget is a simple input extension to allow storing telemetry channel.
         */
        class TlmInput : public Fl_Value_Input
        {
        public:
            /**
             * \brief Constructor
             */
            TlmInput(int x, int y, int w, int h, const char *l = 0) :
                Fl_Value_Input(x, y, w, h, l),
                code(CHANNEL_INVALID)
            {}

            /**
             * \brief Destructor
             */
            ~TlmInput() {}

            /**
             * \brief Get widget telemetry channel
             *
             * \return Telemetry channel code
             */
            ChannelCode get_channel() const {return code;}

            /**
             * \brief Set widget telemetry channel
             *
             * \param code Telemetry channel code
             */
            void set_channel(ChannelCode code) {this->code = code;}

        private:
            ChannelCode code; //!< Telemetry channel code
        };

        /**
         * \brief Power distribution module (PDM) switch control button
         *
         * This widget is a simple button extension that allows storing switch number.
         */
        class SwitchButton : public Fl_Light_Button
        {
        public:
            /**
             * \brief Constructor
             */
            SwitchButton(int x, int y, int w, int h, const char *l = 0) :
                Fl_Light_Button(x, y, w, h, l),
                num(0)
            {}

            /**
             * \brief Destructor
             */
            ~SwitchButton() {}

            /**
             * \brief Get widget PDM switch number
             *
             * \return PDM switch number
             */
            unsigned int get_switch_num() const {return num;}

            /**
             * \brief Set widget PDM switch number
             *
             * \param num PDM switch number
             */
            void set_switch_num(unsigned int num) {this->num = num;}

        private:
            unsigned int num; //!< PDM switch number
        };

        /**
         * \brief Telemetry widgets for each channel
         */
        struct TlmWidgets
        {
            Fl_Value_Output *digital_out; //!< Digital (sampled) telemetry output
            Fl_Value_Output *analog_out;  //!< Analog telemetry output
            TlmInput *analog_in;          //!< Analog telemetry input
        };

        typedef std::map<ChannelCode, TlmWidgets> TlmWidgetMap; //!< Telemetry widget map
    }
}

#endif

