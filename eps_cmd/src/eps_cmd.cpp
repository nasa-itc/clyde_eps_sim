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

#include <ItcLogger/Logger.hpp>
#include <Transport/TransportHub.hpp>
#include <Server/Server.hpp>
#include <Client/Bus.hpp>

// NOTE: fltk includes X11/X.h, which has '#define Success', which causes issues when
//       including nos engine headers that define a 'Success' enum field. to prevent
//       this, the fltk headers are included after nos.
#include "cmd_win.hpp"
#include <Fl/Fl.H>

#include <cstdint>
#include <string>
#include <thread>

/* nos engine connections */
const std::string SERVER_URI = "tcp://127.0.0.1:12000";

/* nos engine buses */
const std::string I2C_BUS  = "i2c_0";
const std::string TIME_BUS = "command";

/* time tick frequency */
const double NOS_TIME_SECONDS = 1.0;

/* eps i2c address */
const uint8_t EPS_I2C_ADDRESS = 0x2b;

/* update nos time */
void timer_cb(void *data)
{
    static uint64_t time_s = 0;
    NosEngine::Client::Bus *bus = reinterpret_cast<NosEngine::Client::Bus*>(data);
    bus->set_time(time_s);
    time_s += NOS_TIME_SECONDS;
    Fl::repeat_timeout(NOS_TIME_SECONDS, timer_cb, data);
}

int main(int argc, char **argv)
{
    // create and start nos engine server
    NosEngine::Transport::TransportHub hub;
    NosEngine::Server::Server server(hub);
    server.add_transport(SERVER_URI);

    // create i2c master
    NosEngine::I2C::I2CMaster master(10, SERVER_URI, I2C_BUS);

    // create nos engine time bus and node
    NosEngine::Client::Bus bus(hub, SERVER_URI, TIME_BUS);
    bus.enable_set_time();

    // create command window
    Fl::visual(FL_DOUBLE | FL_INDEX);
    CommandWindow win(EPS_I2C_ADDRESS, master);
    win.show();

    // create timer callback
    Fl::add_timeout(NOS_TIME_SECONDS, timer_cb, &bus);

    // run fltk event loop
    return Fl::run();
}

