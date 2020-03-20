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

#include "config.hpp"
#include "util.hpp"
#include "bcr.hpp"
#include "pdm.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <algorithm>

using namespace itc::eps;

Config::Config(const std::string& cfgfile) :
    log_level(),
    swap(),
    nos(),
    eps_address(),
    version(),
    db_connected(false),
    db_version(),
    switch_states(),
    tlm(),
    adc()
{
    // load config file
    load(cfgfile);
}

void Config::load(const std::string& cfgfile)
{
    if(cfgfile.empty()) return;

    int index;
    boost::property_tree::ptree cfg;
    boost::property_tree::read_json(cfgfile, cfg);

    // logger
    log_level = cfg.get("log_level", "error");

    // byte swapping
    swap.in = cfg.get("swap_bytes.in", false);
    swap.out = cfg.get("swap_bytes.out", false);

    // nos engine
    nos.uri = cfg.get("nos.uri", "");
    nos.node = cfg.get("nos.node", "");
    nos.i2c_bus = cfg.get("nos.i2c_bus", "");
    nos.time_bus = cfg.get("nos.time_bus", "");
    nos.tick_ms = cfg.get("nos.tick_ms", 0);

    // i2c address
    eps_address = cfg.get<uint8_t>("eps.address", 0);

    // board version
    version.set_version(cfg.get("eps.version.firmware", 0),
                        cfg.get("eps.version.revision", 0));

    // daughterboard version
    db_connected = cfg.get<bool>("eps.daughterboard.connected", false);
    db_version.set_version(cfg.get("eps.daughterboard.firmware", 0),
                           cfg.get("eps.daughterboard.revision", 0));

    // power distribution module (pdm) switch states
    switch_states = get_config_array<bool>(cfg.get_child("eps.switch"), 10);

    // analog telemetry (channel) data
    BOOST_FOREACH(boost::property_tree::ptree::value_type &val, cfg.get_child("eps.tlm"))
    {
        ChannelCode channel = static_cast<ChannelCode>(from_string<uint16_t>(val.first, true));

        // default telemetry value
        tlm[channel] = val.second.get("value", 0.0);
        
        // analog to digital conversion params
        ConverterParams params = get_config_array<double>(val.second.get_child("adc"), 2);
        adc[channel] = params;
    }
}

