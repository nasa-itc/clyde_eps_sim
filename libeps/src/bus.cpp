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

#include "bus.hpp"

using namespace itc::eps;

Bus::Bus(const std::string& name) :
    name(name),
    parent_buses(),
    child_buses(),
    reset_state(false)
{
}

Bus::~Bus()
{
}

void Bus::set_name(const std::string& name)
{
    this->name = name;
}

std::string Bus::get_name() const
{
    return name;
}

void Bus::connect(Bus& bus)
{
    child_buses.insert(&bus);
    bus.add_parent(*this);
}

bool Bus::is_reset() const
{
    return reset_state;
}

void Bus::reset(bool state)
{
    // if reset state change
    if(reset_state != state)
    {
        // check if any parent bus is in reset
        bool parent_reset = false;
        for(BusSet::const_iterator it = parent_buses.begin(); it != parent_buses.end(); ++it)
        {
            if((*it)->is_reset())
            {
                parent_reset = true;
                break;
            }
        }

        // if reset or no parent bus in chain is in reset
        if(state || (!state && !parent_reset))
        {
            // reset all child buses
            reset_state = state;
            for(BusSet::iterator it = child_buses.begin(); it != child_buses.end(); ++it)
            {
                (*it)->reset(reset_state);
            }

            // notify reset state change
            on_reset(reset_state);
        }
    }
}

void Bus::add_parent(Bus& bus)
{
    parent_buses.insert(&bus);
}

