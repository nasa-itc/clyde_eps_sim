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

#include "pdm.hpp"

using namespace itc::eps;

PdmBus::PdmBus() :
    Bus(),
    initial_state(false),
    state(false),
    timer_limit(0xff),
    time_ms(0),
    start_ms(0),
    data()
{
}

PdmBus::~PdmBus()
{
}

void PdmBus::set_time(SimTime time)
{
    time_ms = time;
    
    // check auto-off time
    if(is_timer_active() && (time_ms >= get_off_time()))
    {
        set_state(false);
    }
}

bool PdmBus::get_initial_state() const
{
    return initial_state;
}

void PdmBus::set_initial_state(bool state)
{
    initial_state = state;
}

bool PdmBus::get_state() const
{
    return is_switch_disabled() ? false : state;
}

void PdmBus::set_state(bool state)
{
    // activate tlm channel if switch on and not in reset
    bool active = false;

    this->state = state;
    if(this->state && !is_reset())
    {
        // verify not permanently disabled
        if(!is_switch_disabled())
        {
            // reset timer
            if(is_timer_active()) start_ms = time_ms;

            active = true;
        }
    }
    data.voltage.set_active(active);
    data.current.set_active(active);
}

uint8_t PdmBus::get_timer_limit() const
{
    return timer_limit;
}

void PdmBus::set_timer_limit(uint8_t limit)
{
    timer_limit = limit;

    // permanently disable
    if(is_switch_disabled())
    {
        set_state(false);
    }

    // reset timer if switch enabled
    // TODO should timer be reset when limit changed?
    if(state) start_ms = time_ms;
}

uint8_t PdmBus::get_timer_value() const
{
    // timer limit specified in increments of 30 seconds
    double delta_s = (time_ms - start_ms) / 1000.0;
    return static_cast<uint8_t>(delta_s / 30.0);
}

PdmData* PdmBus::get_data()
{
    return &data;
}

bool PdmBus::is_switch_disabled() const
{
    return timer_limit == 0;
}

bool PdmBus::is_timer_active() const
{
    return (timer_limit > 0) && (timer_limit < 0xff);
}

SimTime PdmBus::get_off_time() const
{
    // timer limit specified in increments of 30 seconds
    return start_ms + (timer_limit * 30 * 1000);
}

void PdmBus::on_reset(bool state)
{
    // activate tlm channel if switch on and not in reset
    bool active = false;
    if(this->state && !state)
    {
        // TODO switch state should equal initial state after reset

        // verify not permanently disabled
        if(!is_switch_disabled())
        {
            // reset timer
            if(is_timer_active()) start_ms = time_ms;

            active = true;
        }
    }
    data.voltage.set_active(active);
    data.current.set_active(active);
}

