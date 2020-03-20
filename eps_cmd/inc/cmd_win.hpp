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

#ifndef COMMAND_WINDOW_HPP
#define COMMAND_WINDOW_HPP

#include "command.hpp"

#include <I2C/Client/I2CMaster.hpp>

#include <Fl/Fl_Window.H>
#include <Fl/Fl_Choice.H>
#include <Fl/Fl_Value_Input.H>
#include <Fl/Fl_Button.H>
#include <Fl/Fl_Output.H>

#include <cstdint>

/**
 * \brief EPS commander window
 */
class CommandWindow : public Fl_Window
{
public:
    /**
     * \brief Constructor
     *
     * \param address EPS simulator I2C address
     * \param master NOS engine I2C master device
     */
    CommandWindow(uint8_t address, NosEngine::I2C::I2CMaster& master);

    /**
     * \brief Destructor
     */
    ~CommandWindow();

private:
    /**
     * \brief Send command callback
     *
     * \param widget Callback widget
     * \param data User data
     */
    static void send_command_cb(Fl_Widget *widget, void *data);

    /**
     * \brief Send command
     */
    void send_command();

private:
    Fl_Choice *cmd_choice;
    Fl_Value_Input *cmd_data_in;
    Fl_Output *cmd_out;
    Fl_Button *send_btn;

    uint8_t address;
    NosEngine::I2C::I2CMaster& master;
};

#endif

