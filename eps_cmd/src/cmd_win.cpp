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

#include "cmd_win.hpp"
#include "types.hpp"
#include "util.hpp"

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

/* command response map */
int COMMANDS[][2] = {
    {itc::eps::CMD_GET_BOARD_STATUS,           2},
    {itc::eps::CMD_GET_LAST_ERROR,             2},
    {itc::eps::CMD_GET_VERSION,                2},
    {itc::eps::CMD_GET_CHECKSUM,               2},
    {itc::eps::CMD_GET_TELEMETRY,              2},
    {itc::eps::CMD_GET_WDT_PERIOD,             2},
    {itc::eps::CMD_SET_WDT_PERIOD,             0},
    {itc::eps::CMD_RESET_WDT,                  0},
    {itc::eps::CMD_GET_NUM_BROWN_OUT_RESETS,   2},
    {itc::eps::CMD_GET_NUM_AUTO_SW_RESETS,     2},
    {itc::eps::CMD_GET_NUM_MANUAL_RESETS,      2},
    {itc::eps::CMD_GET_NUM_WDT_RESETS,         2},
    {itc::eps::CMD_SET_PDM_ALL_ON,             0},
    {itc::eps::CMD_SET_PDM_ALL_OFF,            0},
    {itc::eps::CMD_GET_PDM_ALL_ACTUAL_STATE,   4},
    {itc::eps::CMD_GET_PDM_ALL_EXPECTED_STATE, 4},
    {itc::eps::CMD_GET_PDM_ALL_INITIAL_STATE,  4},
    {itc::eps::CMD_SET_PDM_ALL_INITIAL_STATE,  4},
    {itc::eps::CMD_SET_PDM_ON,                 0},
    {itc::eps::CMD_SET_PDM_OFF,                0},
    {itc::eps::CMD_SET_PDM_INITIAL_STATE_ON,   0},
    {itc::eps::CMD_SET_PDM_INITIAL_STATE_OFF,  0},
    {itc::eps::CMD_GET_PDM_ACTUAL_STATE,       2},
    {itc::eps::CMD_SET_PDM_TIMER_LIMIT,        0},
    {itc::eps::CMD_GET_PDM_TIMER_LIMIT,        2},
    {itc::eps::CMD_GET_PDM_TIMER_VALUE,        2},
    {itc::eps::CMD_SET_PCM_RESET,              0},
    {itc::eps::CMD_RESET_NODE,                 0},
    {static_cast<itc::eps::CommandType>(0xff), 0}
};

CommandWindow::CommandWindow(uint8_t address, NosEngine::I2C::I2CMaster& master) :
    Fl_Window(380, 150, "EPS Commander"),
    cmd_choice(NULL),
    cmd_data_in(NULL),
    cmd_out(NULL),
    send_btn(NULL),
    address(address),
    master(master)
{
    begin();

    cmd_choice = new Fl_Choice(100, 20, 250, 25, "Command");
    int num_commands = sizeof(COMMANDS) / sizeof(COMMANDS[0]);
    for(unsigned int i = 0; i < num_commands; i++)
    {
        itc::eps::CommandType type = static_cast<itc::eps::CommandType>(COMMANDS[i][0]);
        cmd_choice->add(itc::eps::to_string(type).c_str());
    }
    cmd_choice->value(0);

    cmd_data_in = new Fl_Value_Input(100, 50, 250, 25, "Data");
    cmd_data_in->step(1);

    cmd_out = new Fl_Output(100, 80, 250, 25, "Response");

    send_btn = new Fl_Button(270, 110, 80, 25, "Send");
    send_btn->callback(CommandWindow::send_command_cb, this);

    end();
}

CommandWindow::~CommandWindow()
{
}

void CommandWindow::send_command_cb(Fl_Widget* /*widget*/, void *data)
{
    static_cast<CommandWindow*>(data)->send_command();
}

void CommandWindow::send_command()
{
    itc::eps::CommandType type = static_cast<itc::eps::CommandType>(COMMANDS[cmd_choice->value()][0]);
    uint16_t param = static_cast<uint16_t>(cmd_data_in->value());
    size_t rlen = COMMANDS[cmd_choice->value()][1];

    itc::eps::I2CData msg{static_cast<unsigned char>(type)};
    if((type == itc::eps::CMD_GET_TELEMETRY) || (type == itc::eps::CMD_SET_PDM_TIMER_LIMIT))
    {
        msg.push_back((param >> 8) & 0xff);
        msg.push_back(param & 0xff);
    }
    else
    {
        msg.push_back(static_cast<uint8_t>(param));
    }

    itc::eps::I2CData resp(rlen, 0);
    NosEngine::I2C::Result result = master.i2c_transaction(address, &msg[0], msg.size(), &resp[0], resp.size());

    std::string output = "NO RESPONSE";
    if((result == NosEngine::I2C::Result::I2C_SUCCESS) && (rlen > 0)) 
    {
        std::stringstream ss;
        ss << "0x";
        for(itc::eps::I2CData::const_iterator it = resp.begin(); it != resp.end(); ++it)
        {
            ss << std::setfill('0') << std::setw(2) << std::hex
               << (static_cast<unsigned int>(*it) & 0xff);
        }
        output = ss.str();
    }
    
    cmd_out->value(output.c_str());
}

