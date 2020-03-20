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

#include "eps.hpp"
#include "config.hpp"
#include "eps_sim.hpp"

#include <ItcLogger/Logger.hpp>

#include <Fl/Fl.H>
#include <boost/program_options.hpp>

#include <iostream>
#include <string>

namespace
{
    ItcLogger::Logger *logger = ItcLogger::Logger::get(itc::eps::LOGGER_NAME.c_str());
}

/* parse command line */
bool parse_command_line(int argc, char **argv, std::string& cfgfile, bool& iconized)
{
    namespace po = boost::program_options;

    // command line options
    po::variables_map eps_opts;
    po::options_description eps_desc("STF-1 EPS Simulator Options");
    eps_desc.add_options()
        ("help,h", "display help")
        ("config,c", po::value<std::string>(&cfgfile), "eps config file (json)")
        ("iconic,i", "start iconized (default=false)");

    // parse command line
    try
    {
        po::store(po::parse_command_line(argc, argv, eps_desc), eps_opts);
        po::notify(eps_opts);
    }
    catch(const po::error& e)
    {
        std::cerr << "eps command line parse error: " << e.what() << std::endl;
        return false;
    }

    // print help
    if(eps_opts.count("help"))
    {
        std::cout << eps_desc;
        return false;
    }

    iconized = false;
    if(eps_opts.count("iconic")) iconized = true;

    // verify required parameter
    if(cfgfile.empty())
    {
        std::cerr << "missing required --config option" << std::endl;
        return false;
    }

    return true;
}

/* configure logger */
void configure_logger(const std::string& level)
{
    // create default stdio logger
    logger->set_level(ItcLogger::String2Level(level.c_str()));
    ItcLogger::TargetPtr target(new ItcLogger::Target(STDIO_BUILTIN_TARGET_IMPL, ItcLogger::LogArguments()));
    target->set_format("%t [%l] - %s");
    logger->add_target(target);
}

int main(int argc, char **argv)
{
    // parse command line
    std::string cfgfile;
    bool iconized = false;
    if(!parse_command_line(argc, argv, cfgfile, iconized)) return 1;

    // load config
    itc::eps::Config config(cfgfile);

    // initialize logger
    configure_logger(config.log_level);

    // create and configure eps simulator
    logger->info("creating eps sim: cfg=%s", cfgfile.c_str());
    itc::eps::EpsSim sim(config);
    
    // minimize window
    if(iconized) sim.minimize();

    // enable threading and run event loop
    Fl::lock();
    Fl::visual(FL_DOUBLE | FL_INDEX);
    return Fl::run();
}

