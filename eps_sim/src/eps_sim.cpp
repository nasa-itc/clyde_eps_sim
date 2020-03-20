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

#include "eps_sim.hpp"
#include "types.hpp"
#include "util.hpp"

#include <Client/Bus.hpp>
#include <ItcLogger/Logger.hpp>

// NOTE: fltk includes X11/X.h, which has '#define Success', which causes issues when
//       including nos engine headers that define a 'Success' enum field. to prevent
//       this, the fltk headers are included after nos.
#include "eps_win.hpp"
#include "widgets.hpp"

#include <cstdint>
#include <algorithm>
#include <functional>

using namespace itc::eps;

ItcLogger::Logger *logger = ItcLogger::Logger::get(itc::eps::LOGGER_NAME.c_str());

const ChannelCode SWITCH_CHANNELS[][2] = {
    {CHANNEL_VSW1,  CHANNEL_ISW1},
    {CHANNEL_VSW2,  CHANNEL_ISW2},
    {CHANNEL_VSW3,  CHANNEL_ISW3},
    {CHANNEL_VSW4,  CHANNEL_ISW4},
    {CHANNEL_VSW5,  CHANNEL_ISW5},
    {CHANNEL_VSW6,  CHANNEL_ISW6},
    {CHANNEL_VSW7,  CHANNEL_ISW7},
    {CHANNEL_VSW8,  CHANNEL_ISW8},
    {CHANNEL_VSW9,  CHANNEL_ISW9},
    {CHANNEL_VSW10, CHANNEL_ISW10},
};

EpsSim::EpsSim(const Config& config) :
    NosEngine::I2C::I2CSlave(config.eps_address, config.nos.uri, config.nos.i2c_bus),
    mutex(),
    time_bus(get_transport_hub(), config.nos.uri, config.nos.time_bus),
    tick_ms(config.nos.tick_ms),
    eps(config.eps_address, config.db_connected, config.swap),
    win(new EpsWindow)
{
    // create time client
    //time_bus.add_time_tick_callback(std::bind(&EpsSim::on_time_tick, this, std::placeholders::_1);

    // setup gui callbacks
    for(TlmWidgetMap::iterator it = win->tlm.begin(); it != win->tlm.end(); ++it)
    {
        it->second.analog_in->set_channel(it->first);
        it->second.analog_in->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
        it->second.analog_in->callback(on_tlm_update, this);
    }
    for(int i = 0; i < NUM_SWITCHES; i++)
    {
        win->switch_in[i]->set_switch_num(i);
        win->switch_in[i]->callback(on_switch_update, this);
    }

    // show window
    win->nos_status_out->value(true);
    win->show();

    // initialize eps simulator
    eps.set_version(config.version);
    eps.set_daughterboard_version(config.db_version);
    for(Config::Telemetry::const_iterator it = config.tlm.begin(); it != config.tlm.end(); ++it)
    {
        eps.set_telemetry(it->first, it->second);
    }
    for(Config::ChannelConfig::const_iterator it = config.adc.begin(); it != config.adc.end(); ++it)
    {
        eps.configure_channel(it->first, it->second);
    }
    for(int i = 0; i < config.switch_states.size(); i++)
    {
        eps.set_switch_state(i, config.switch_states[i]);
    }

    // set intial window state
    update_win();
}

EpsSim::~EpsSim()
{
    if(win) delete win;
}

void EpsSim::minimize()
{
    win->iconize();
}

size_t EpsSim::i2c_read(uint8_t* rbuf, size_t rlen)
{
    // lock for the eps sim object
    std::lock_guard<std::mutex> lock(mutex);

    // eps i2c transaction
    itc::eps::I2CData response;
    eps.i2c_read(response);

    size_t num_read = response.size();
    if(num_read > rlen)
    {
        response.resize(rlen);
    }

    std::copy(response.begin(), response.end(), rbuf);

    return num_read;
}

size_t EpsSim::i2c_write(const uint8_t *wbuf, size_t wlen)
{
    // lock for the eps sim object
    std::lock_guard<std::mutex> lock(mutex);

    // update eps time
    uint64_t time_ms = time_bus.get_time() * tick_ms;
    eps.set_time(time_ms);

    logger->info("nos request received: time=%lums", static_cast<unsigned long>(time_ms));
    
    // eps i2c transaction
    itc::eps::I2CData data(wbuf, wbuf + wlen);
    eps.i2c_write(data);

    // update ui
    update_win();

    return wlen;
}

void EpsSim::on_time_tick(NosEngine::Common::SimTime time)
{
    logger->info("on_time_tick: %lu", static_cast<unsigned long>(time));
}

void EpsSim::on_tlm_update(Fl_Widget *widget, void *user)
{
    EpsSim *sim = reinterpret_cast<EpsSim*>(user);
    TlmInput *input = reinterpret_cast<TlmInput*>(widget);

    // lock for the eps sim object
    std::lock_guard<std::mutex> lock(sim->mutex);

    // update telemetry
    ChannelCode code = input->get_channel();
    double val = input->value();
    sim->eps.set_telemetry(code, val);

    // reset input widget
    input->value(0);

    // read actual value to ensure successful update
    ChannelTelemetry tlm;
    sim->eps.get_telemetry(code, tlm);
    TlmWidgets widgets = sim->win->tlm[code];
    widgets.digital_out->value(tlm.digital);
    widgets.analog_out->value(tlm.analog);
}

void EpsSim::on_switch_update(Fl_Widget *widget, void *user)
{
    EpsSim *sim = reinterpret_cast<EpsSim*>(user);
    SwitchButton *btn = reinterpret_cast<SwitchButton*>(widget);

    // lock for the eps sim object
    std::lock_guard<std::mutex> lock(sim->mutex);

    // update switch state
    unsigned int num = btn->get_switch_num();
    bool state = btn->value();
    sim->eps.set_switch_state(num, state);

    // reset switch state
    btn->value(sim->eps.get_switch_state(num));

    // update switch telemetry values
    for(int i = 0; i < 2; i++)
    {
        ChannelCode code = SWITCH_CHANNELS[num][i];
        ChannelTelemetry tlm;
        sim->eps.get_telemetry(code, tlm);
        TlmWidgets widgets = sim->win->tlm[code];
        widgets.digital_out->value(tlm.digital);
        widgets.analog_out->value(tlm.analog);
    }
}

void EpsSim::update_win()
{
    // ensure proper locking for thread support
    Fl::lock();

    // update sim time
    win->sim_time_out->value(eps.get_time() / 1000.0); 

    // update version
    Version version = eps.get_version();
    win->firmware_out->value(version.get_firmware());
    win->revision_out->value(version.get_revision());

    // update status
    Status status = eps.get_status();
    win->checksum_out->value(to_string(status.get_checksum(), true).c_str());
    win->reset_power_on_out->value(status.get_num_resets(RESET_POWER_ON));
    win->reset_brown_out->value(status.get_num_resets(RESET_BROWN_OUT));
    win->reset_manual_out->value(status.get_num_resets(RESET_MANUAL));
    win->reset_wdt_out->value(status.get_num_resets(RESET_WDT));
    for(int i = 0; i < NUM_STATUS_BITS; i++)
    {
        win->status_out[i]->value(status.is_set(static_cast<BoardStatus>(i)));
    }
    ErrorCode ec = status.get_last_error();
    win->error_code_out->value(to_string(ec, true).c_str());
    win->error_msg_out->value(to_string(ec).c_str());

    // update switch states
    for(int i = 0; i < NUM_SWITCHES; i++)
    {
        win->switch_in[i]->value(eps.get_switch_state(i));
    }

    // update telemetry
    Telemetry tlm;
    eps.get_telemetry(tlm);
    for(Telemetry::const_iterator it = tlm.begin(); it != tlm.end(); ++it)
    {
        TlmWidgetMap::const_iterator wit = win->tlm.find(it->first);
        if(wit != win->tlm.end())
        {
            wit->second.digital_out->value(it->second.digital);
            wit->second.analog_out->value(it->second.analog);
        }
    }

    Fl::awake();
    Fl::unlock();
}

