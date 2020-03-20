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
#include "command.hpp"
#include "util.hpp"
#include <ItcLogger/Logger.hpp>
#include <bitset>
#include <algorithm>

using namespace itc::eps;

static ItcLogger::Logger *logger = ItcLogger::Logger::get(LOGGER_NAME.c_str());

Eps::Eps(uint8_t address, bool daughterboard, ByteSwapConfig swap_config) :
    swap(swap_config),
    address(address),
    response(),
    time_ms(0),
    version(),
    status(),
    bcr_bus(),
    pcm_bus(),
    pdm_bus(),
    db_connected(daughterboard),
    db_version(),
    db_status(),
    misc_channels(),
    adc(),
    wdt_time_ms(0),
    wdt_timeout_ms(DEFAULT_WDT_TIMEOUT_MS),
    reset_buses()
{
    // name buses to improve logging
    bcr_bus.set_name("BCR");
    for(int i = 0; i < NUM_PCM_BUSES; i++)
    {
        PcmBusType type = static_cast<PcmBusType>(i);
        pcm_bus[type].set_name(to_string(type));
    }
    for(int i = 0; i < NUM_SWITCHES; i++)
    {
        pdm_bus[i].set_name("PDM_SWITCH_" + to_string(i));
    }

    // connect analog telemetry channels
    connect_channels(); 

    // connect power buses (to propagate reset signals)
    connect_buses();
}

Eps::~Eps()
{
}

void Eps::i2c_write(const I2CData& data)
{
    // no response if in reset
    response.clear();
    if(is_reset()) return;

    // verify command
    if(data.size() < 2)
    {
        logger->error("invalid eps command");
        set_response(CMD_RESP_ERROR);
        return;
    }

    bool cmd_valid = true;
    bool cmd_data_valid = true;
    bool cmd_channel_valid = true;

    CommandType type = static_cast<CommandType>(data[0]);
    uint32_t param = get_command_param(data);

    logger->info("eps cmd %s: cmd=0x%x, param=0x%x", to_string(type).c_str(), static_cast<uint8_t>(type), param);

    // data/channel range checks
    CommandDataRangeMap::const_iterator it;

    it = CMD_DATA_RANGES.find(type); 
    cmd_data_valid = (it != CMD_DATA_RANGES.end()) ? it->second.is_valid(param) : true;
    if(!cmd_data_valid) logger->error("eps cmd data out of range");

    it = CMD_CHANNEL_RANGES.find(type); 
    cmd_channel_valid = (it != CMD_CHANNEL_RANGES.end()) ? it->second.is_valid(param) : true;
    if(!cmd_channel_valid) logger->error("eps cmd channel out of range");
    
    // execute commands
    if(cmd_data_valid && cmd_channel_valid)
    {
        switch(type)
        {
            case CMD_GET_BOARD_STATUS:
            {
                if(db_connected)
                {
                    uint32_t stat = (static_cast<uint32_t>(db_status.get_status()) << 16) |
                                     static_cast<uint32_t>(status.get_status());
                    set_response(stat);
                }
                else
                {
                    set_response(status.get_status());
                }
                break;
            }
            case CMD_GET_LAST_ERROR:
                if(db_connected)
                {
                    uint32_t error = (static_cast<uint32_t>(db_status.get_last_error()) << 16) |
                                      static_cast<uint32_t>(status.get_last_error());
                    set_response(error);
                }
                else
                {
                    set_response(static_cast<uint16_t>(status.get_last_error()));
                }
                break;
            case CMD_GET_VERSION:
                if(db_connected)
                {
                    uint32_t vers = (static_cast<uint32_t>(db_version.version) << 16) |
                                     static_cast<uint32_t>(version.version);
                    set_response(vers);
                }
                else
                {
                    set_response(version.version);
                }
                break;
            case CMD_GET_CHECKSUM:
                if(db_connected)
                {
                    uint32_t xsum = (static_cast<uint32_t>(db_status.get_checksum()) << 16) |
                                     static_cast<uint32_t>(db_status.get_checksum());
                    set_response(xsum);
                }
                else
                {
                    set_response(status.get_checksum());
                }
                break;
            case CMD_GET_TELEMETRY:
                cmd_channel_valid = is_channel_valid(static_cast<uint16_t>(param));
                set_response(read_telemetry(static_cast<uint16_t>(param))); 
                break;
            case CMD_GET_WDT_PERIOD:
                set_response(static_cast<uint16_t>(wdt_timeout_ms / (1000 * 60.0)));
                break;
            case CMD_SET_WDT_PERIOD:
                wdt_timeout_ms = param * 60 * 1000; // data provided in minutes
                break;
            case CMD_RESET_WDT:
                break;
            case CMD_GET_NUM_BROWN_OUT_RESETS:
                if(db_connected)
                {
                    uint32_t resets = (static_cast<uint32_t>(db_status.get_num_resets(RESET_BROWN_OUT)) << 16) |
                                       static_cast<uint32_t>(status.get_num_resets(RESET_BROWN_OUT));

                    set_response(resets);
                }
                else
                {
                    set_response(static_cast<uint16_t>(status.get_num_resets(RESET_BROWN_OUT)));
                }
                break;
            case CMD_GET_NUM_AUTO_SW_RESETS:
                if(db_connected)
                {
                    set_response(uint32_t(0));
                }
                else
                {
                    set_response(uint16_t(0));
                }
                break;
            case CMD_GET_NUM_MANUAL_RESETS:
                if(db_connected)
                {
                    uint32_t resets = (static_cast<uint32_t>(db_status.get_num_resets(RESET_MANUAL)) << 16) |
                                       static_cast<uint32_t>(status.get_num_resets(RESET_MANUAL));

                    set_response(resets);
                }
                else
                {
                    set_response(static_cast<uint16_t>(status.get_num_resets(RESET_MANUAL)));
                }
                break;
            case CMD_GET_NUM_WDT_RESETS:
                set_response(static_cast<uint16_t>(status.get_num_resets(RESET_WDT)));
                break;
            case CMD_SET_PDM_ALL_ON:
                set_pdm_all(false, true);
                break;
            case CMD_SET_PDM_ALL_OFF:
                set_pdm_all(false, false);
                break;
            case CMD_GET_PDM_ALL_ACTUAL_STATE:
                set_response(get_pdm_all(false));
                break;
            case CMD_GET_PDM_ALL_EXPECTED_STATE:
                set_response(get_pdm_all(false));
                break;
            case CMD_GET_PDM_ALL_INITIAL_STATE:
                set_response(get_pdm_all(true));
                break;
            case CMD_SET_PDM_ALL_INITIAL_STATE:
                // TODO how is the state flag sent??
                set_pdm_all(true, true);
                break;
            case CMD_SET_PDM_ON:
                pdm_bus[param-1].set_state(true);
                break;
            case CMD_SET_PDM_OFF:
                pdm_bus[param-1].set_state(false);
                break;
            case CMD_SET_PDM_INITIAL_STATE_ON:
                pdm_bus[param-1].set_initial_state(true);
                break;
            case CMD_SET_PDM_INITIAL_STATE_OFF:
                pdm_bus[param-1].set_initial_state(false);
                break;
            case CMD_GET_PDM_ACTUAL_STATE:
                set_response(static_cast<uint16_t>(pdm_bus[param-1].get_state() ? 1 : 0));
                break;
            case CMD_SET_PDM_TIMER_LIMIT:
                set_pdm_timer_limit(static_cast<uint16_t>(param));
                break;
            case CMD_GET_PDM_TIMER_LIMIT:
                set_response(static_cast<uint16_t>(pdm_bus[param-1].get_timer_limit()));
                break;
            case CMD_GET_PDM_TIMER_VALUE:
                set_response(static_cast<uint16_t>(pdm_bus[param-1].get_timer_value()));
                break;
            case CMD_SET_PCM_RESET:
                set_pcm_reset(static_cast<uint8_t>(param));
                break;
            case CMD_RESET_NODE:
                // TODO proper reset
                status.set(RESET_MANUAL);
                reset_bus(bcr_bus);
                wdt_time_ms = 0;
                break;
            default:
                logger->error("unknown eps command");
                cmd_valid = false;
                break;
        }
    }

    // set last command validity flags
    status.set(STATUS_INVALID_CMD, !cmd_valid); 
    status.set(STATUS_INVALID_DATA, !cmd_data_valid); 
    status.set(STATUS_INVALID_CHANNEL, !cmd_channel_valid); 

    // error response
    bool valid = (cmd_valid && cmd_data_valid && cmd_channel_valid);
    if(!valid) set_response(CMD_RESP_ERROR);

    // reset wdt on valid command
    // TODO should this be reset on any traffic or only valid commands?
    if(valid) wdt_time_ms = 0;
}

void Eps::i2c_read(I2CData& data)
{
    data = response;
}

uint8_t Eps::get_address() const
{
    return address;
}

SimTime Eps::get_time() const
{
    return time_ms;
}

void Eps::set_time(SimTime time)
{
    // set watchdog timer time
    if(!is_reset()) wdt_time_ms += (time - time_ms);

    // set current sim time
    time_ms = time;

    // set pdm bus times for auto-shutoff timers
    for(int i = 0; i < NUM_SWITCHES; i++)
    {
        pdm_bus[i].set_time(time);
    }

    // find all buses with expired reset times and release reset
    ResetBusSet::iterator erase_it = reset_buses.upper_bound(ResetInfo(NULL, time_ms));
    for(ResetBusSet::iterator it = reset_buses.begin(); it != erase_it; ++it)
    {
        logger->info("bus %s reset disabled (time=%fs)", it->bus->get_name().c_str(), time_ms/1000.0);
        it->bus->reset(false);
    }

    // remove expired buses from set
    reset_buses.erase(reset_buses.begin(), erase_it);

    // check state of watchdog timer
    if(wdt_time_ms >= wdt_timeout_ms)
    {
        logger->warning("watchdog timer reset (time=%fs)", time_ms/1000.0);
        wdt_time_ms = 0;
        reset_bus(bcr_bus);
        status.set(RESET_WDT);
    }
}

Version Eps::get_version() const
{
    return version;
}

void Eps::set_version(const Version& version)
{
    this->version = version;
}

Status Eps::get_status() const
{
    return status;
}

void Eps::set_status(const Status& status)
{
    this->status = status;
}

Version Eps::get_daughterboard_version() const
{
    return db_version;
}

void Eps::set_daughterboard_version(const Version& version)
{
    this->db_version = version;
}

Status Eps::get_daughterboard_status() const
{
    return db_status;
}

void Eps::set_daughterboard_status(const Status& status)
{
    this->db_status = status;
}

void Eps::get_telemetry(Telemetry& tlm) const
{
    tlm.clear();
    for(Converter::const_iterator it = adc.begin(); it != adc.end(); ++it)
    {
        ChannelTelemetry& channel_tlm = tlm[it->first];
        channel_tlm.digital = it->second->sample();
        channel_tlm.analog = it->second->get_value();
    }
}

void Eps::get_telemetry(ChannelCode code, ChannelTelemetry& tlm) const
{
    Converter::const_iterator it = adc.find(code);
    if(it != adc.end())
    {
        tlm.digital = it->second->sample();
        tlm.analog = it->second->get_value();
    }
    else
    {
        logger->error("invalid telemetry channel: 0x%x", code);
    }
}

void Eps::set_telemetry(ChannelCode code, double val)
{
    logger->info("updating eps telemetry channel: 0x%x", code);

    Converter::iterator it = adc.find(code);
    if(it != adc.end())
    {
        it->second->set_value(val);
    }
    else
    {
        logger->error("invalid telemetry channel: 0x%x", code);
    }
}

bool Eps::get_switch_state(unsigned int num) const
{
    bool state = false;
    if(num < NUM_SWITCHES)
    {
        state = pdm_bus[num].get_state();
    }
    else
    {
        logger->error("invalid switch number: %d", num);
    }
    return state;
}

void Eps::set_switch_state(unsigned int num, bool on)
{
    logger->info("updating eps pdm switch %d state: %s", num, on ? "on" : "off");

    if(num < NUM_SWITCHES)
    {
        pdm_bus[num].set_state(on);
    }
    else
    {
        logger->error("invalid switch number: %d", num);
    }
}

bool Eps::get_switch_initial_state(unsigned int num) const
{
    bool state = false;
    if(num < NUM_SWITCHES)
    {
        state = pdm_bus[num].get_initial_state();
    }
    else
    {
        logger->error("invalid switch number: %d", num);
    }
    return state;
}

void Eps::set_switch_initial_state(unsigned int num, bool on)
{
    logger->info("updating eps pdm switch %d initial state: %s", num, on ? "on" : "off");

    if(num < NUM_SWITCHES)
    {
        pdm_bus[num].set_initial_state(on);
    }
    else
    {
        logger->error("invalid switch number: %d", num);
    }
}

bool Eps::is_reset() const
{
    // TODO i2c reset is actually different than bcr - need to read manual but 3.3v reset
    return bcr_bus.is_reset();
}

void Eps::configure_channel(ChannelCode code, const ConverterParams& params)
{
    Converter::iterator it = adc.find(code);
    if(it != adc.end())
    {
        it->second->configure(params);
    }
    else
    {
        logger->error("invalid telemetry channel: 0x%x", code);
    }
}

bool Eps::is_channel_valid(uint16_t code) const
{
    return adc.count(static_cast<ChannelCode>(code));
}

uint16_t Eps::read_telemetry(uint16_t param)
{
    uint16_t data = CMD_RESP_ERROR;
    ChannelCode code = static_cast<ChannelCode>(param);
    Converter::const_iterator it = adc.find(code);
    if(it != adc.end())
    {
        data = it->second->sample();
    }
    return data;
}

void Eps::set_pcm_reset(uint8_t data)
{
    std::bitset<4> pcm_reset(data);

    if(pcm_reset[PCM_BUS_BAT])
    {
        reset_bus(pcm_bus[PCM_BUS_BAT]);
    }

    if(pcm_reset[PCM_BUS_5V])
    {
        reset_bus(pcm_bus[PCM_BUS_5V]);
    }

    if(pcm_reset[PCM_BUS_3V3])
    {
        reset_bus(pcm_bus[PCM_BUS_3V3]);
    }

    if(pcm_reset[PCM_BUS_12V])
    {
        reset_bus(pcm_bus[PCM_BUS_12V]);
    }
}

void Eps::set_pdm_all(bool initial, bool state)
{
    for(int i = 0; i < NUM_SWITCHES; i++)
    {
        if(initial)
        {
            pdm_bus[i].set_initial_state(state);
        }
        else
        {
            pdm_bus[i].set_state(state);
        }
    }
}

uint32_t Eps::get_pdm_all(bool initial) const
{
    uint32_t state = 0;
    for(int i = 0; i < NUM_SWITCHES; i++)
    {
        if(initial)
        {
            state |= (pdm_bus[i].get_initial_state() ? 1 : 0) << i+1;
        }
        else
        {
            state |= (pdm_bus[i].get_state() ? 1 : 0) << i+1;
        }
    }
    return state;
}

void Eps::set_pdm_timer_limit(uint16_t data)
{
    uint8_t period = (data & 0xff);
    uint8_t pdm = ((data >> 8) & 0xff) - 1;
    if(pdm < NUM_SWITCHES)
    {
        pdm_bus[pdm].set_timer_limit(period);
    }
    else
    {
        logger->error("invalid switch number: %d", pdm);
    }
}

uint16_t Eps::get_pcm_state() const
{
    uint16_t state = 0;
    for(int i = 0; i < NUM_PCM_BUSES; i++)
    {
        PcmBusType type = static_cast<PcmBusType>(i);
        if(!pcm_bus[type].is_reset()) state |= 1 << i;
    }
    return state;
}

void Eps::reset_bus(Bus& bus)
{
    if(!bus.is_reset())
    {
        logger->info("bus %s reset enabled (time=%fs)", bus.get_name().c_str(), time_ms/1000.0);
        bus.reset(true);
        reset_buses.insert(ResetInfo(&bus, time_ms + DEFAULT_BUS_RESET_TIME_MS));
    }
    else
    {
        logger->warning("bus %s already in reset state", bus.get_name().c_str());
    }
}

void Eps::connect_channels()
{
    // battery charge regulators (bcr)
    adc[CHANNEL_VBCR1]   = &bcr_bus.get_data(0)->voltage;
    adc[CHANNEL_IBCR1A]  = &bcr_bus.get_data(0)->current[0];
    adc[CHANNEL_IBCR1B]  = &bcr_bus.get_data(0)->current[1];
    adc[CHANNEL_TBCR1A]  = &bcr_bus.get_data(0)->temp[0];
    adc[CHANNEL_TBCR1B]  = &bcr_bus.get_data(0)->temp[1];
    adc[CHANNEL_SDBCR1A] = &bcr_bus.get_data(0)->sun[0];
    adc[CHANNEL_SDBCR1B] = &bcr_bus.get_data(0)->sun[1];

    adc[CHANNEL_VBCR2]   = &bcr_bus.get_data(1)->voltage;
    adc[CHANNEL_IBCR2A]  = &bcr_bus.get_data(1)->current[0];
    adc[CHANNEL_IBCR2B]  = &bcr_bus.get_data(1)->current[1];
    adc[CHANNEL_TBCR2A]  = &bcr_bus.get_data(1)->temp[0];
    adc[CHANNEL_TBCR2B]  = &bcr_bus.get_data(1)->temp[1];
    adc[CHANNEL_SDBCR2A] = &bcr_bus.get_data(1)->sun[0];
    adc[CHANNEL_SDBCR2B] = &bcr_bus.get_data(1)->sun[1];

    adc[CHANNEL_VBCR3]   = &bcr_bus.get_data(2)->voltage;
    adc[CHANNEL_IBCR3A]  = &bcr_bus.get_data(2)->current[0];
    adc[CHANNEL_IBCR3B]  = &bcr_bus.get_data(2)->current[1];
    adc[CHANNEL_TBCR3A]  = &bcr_bus.get_data(2)->temp[0];
    adc[CHANNEL_TBCR3B]  = &bcr_bus.get_data(2)->temp[1];
    adc[CHANNEL_SDBCR3A] = &bcr_bus.get_data(2)->sun[0];
    adc[CHANNEL_SDBCR3B] = &bcr_bus.get_data(2)->sun[1];

    adc[CHANNEL_VBCR4]   = &bcr_bus.get_data(3)->voltage;
    adc[CHANNEL_IBCR4A]  = &bcr_bus.get_data(3)->current[0];
    adc[CHANNEL_IBCR4B]  = &bcr_bus.get_data(3)->current[1];
    adc[CHANNEL_TBCR4A]  = &bcr_bus.get_data(3)->temp[0];
    adc[CHANNEL_TBCR4B]  = &bcr_bus.get_data(3)->temp[1];
    adc[CHANNEL_SDBCR4A] = &bcr_bus.get_data(3)->sun[0];
    adc[CHANNEL_SDBCR4B] = &bcr_bus.get_data(3)->sun[1];

    adc[CHANNEL_VBCR5]   = &bcr_bus.get_data(4)->voltage;
    adc[CHANNEL_IBCR5A]  = &bcr_bus.get_data(4)->current[0];
    adc[CHANNEL_IBCR5B]  = &bcr_bus.get_data(4)->current[1];
    adc[CHANNEL_TBCR5A]  = &bcr_bus.get_data(4)->temp[0];
    adc[CHANNEL_TBCR5B]  = &bcr_bus.get_data(4)->temp[1];
    adc[CHANNEL_SDBCR5A] = &bcr_bus.get_data(4)->sun[0];
    adc[CHANNEL_SDBCR5B] = &bcr_bus.get_data(4)->sun[1];


    // power conditioning module (pcm) buses
    adc[CHANNEL_VPCMBATV] = &pcm_bus[PCM_BUS_BAT].get_data()->voltage;
    adc[CHANNEL_IPCMBATV] = &pcm_bus[PCM_BUS_BAT].get_data()->current;
    adc[CHANNEL_VPCM12V]  = &pcm_bus[PCM_BUS_12V].get_data()->voltage;
    adc[CHANNEL_IPCM12V]  = &pcm_bus[PCM_BUS_12V].get_data()->current;
    adc[CHANNEL_VPCM5V]   = &pcm_bus[PCM_BUS_5V].get_data()->voltage;
    adc[CHANNEL_IPCM5V]   = &pcm_bus[PCM_BUS_5V].get_data()->current;
    adc[CHANNEL_VPCM3V3]  = &pcm_bus[PCM_BUS_3V3].get_data()->voltage;
    adc[CHANNEL_IPCM3V3]  = &pcm_bus[PCM_BUS_3V3].get_data()->current;

    // power distribution module (pdm) switches
    adc[CHANNEL_VSW1]  = &pdm_bus[0].get_data()->voltage;
    adc[CHANNEL_ISW1]  = &pdm_bus[0].get_data()->current;
    adc[CHANNEL_VSW2]  = &pdm_bus[1].get_data()->voltage;
    adc[CHANNEL_ISW2]  = &pdm_bus[1].get_data()->current;
    adc[CHANNEL_VSW3]  = &pdm_bus[2].get_data()->voltage;
    adc[CHANNEL_ISW3]  = &pdm_bus[2].get_data()->current;
    adc[CHANNEL_VSW4]  = &pdm_bus[3].get_data()->voltage;
    adc[CHANNEL_ISW4]  = &pdm_bus[3].get_data()->current;
    adc[CHANNEL_VSW5]  = &pdm_bus[4].get_data()->voltage;
    adc[CHANNEL_ISW5]  = &pdm_bus[4].get_data()->current;
    adc[CHANNEL_VSW6]  = &pdm_bus[5].get_data()->voltage;
    adc[CHANNEL_ISW6]  = &pdm_bus[5].get_data()->current;
    adc[CHANNEL_VSW7]  = &pdm_bus[6].get_data()->voltage;
    adc[CHANNEL_ISW7]  = &pdm_bus[6].get_data()->current;
    adc[CHANNEL_VSW8]  = &pdm_bus[7].get_data()->voltage;
    adc[CHANNEL_ISW8]  = &pdm_bus[7].get_data()->current;
    adc[CHANNEL_VSW9]  = &pdm_bus[8].get_data()->voltage;
    adc[CHANNEL_ISW9]  = &pdm_bus[8].get_data()->current;
    adc[CHANNEL_VSW10] = &pdm_bus[9].get_data()->voltage;
    adc[CHANNEL_ISW10] = &pdm_bus[9].get_data()->current;

    // misc board telemetry
    adc[CHANNEL_IIDIODE]  = &misc_channels[CHANNEL_IIDIODE];
    adc[CHANNEL_VIDIODE]  = &misc_channels[CHANNEL_VIDIODE];
    adc[CHANNEL_I3V3_DRW] = &misc_channels[CHANNEL_I3V3_DRW];
    adc[CHANNEL_I5V_DRW]  = &misc_channels[CHANNEL_I5V_DRW];
    adc[CHANNEL_TBRD]     = &misc_channels[CHANNEL_TBRD];
}

void Eps::connect_buses()
{
    // connect main battery charge regulator (bcr) bus to power conditioning module (pcm) buses
    for(int i = 0; i < NUM_PCM_BUSES; i++)
    {
        bcr_bus.connect(pcm_bus[i]); 
    }

    // connect power conditioning module (pcm) buses to power distribution module (pdm) switch buses
    pcm_bus[PCM_BUS_12V].connect(pdm_bus[0]);
    pcm_bus[PCM_BUS_12V].connect(pdm_bus[1]);
    pcm_bus[PCM_BUS_5V].connect(pdm_bus[2]);
    pcm_bus[PCM_BUS_3V3].connect(pdm_bus[3]);
    pcm_bus[PCM_BUS_5V].connect(pdm_bus[4]);
    pcm_bus[PCM_BUS_5V].connect(pdm_bus[5]);
    pcm_bus[PCM_BUS_5V].connect(pdm_bus[6]);
    pcm_bus[PCM_BUS_3V3].connect(pdm_bus[7]);
    pcm_bus[PCM_BUS_3V3].connect(pdm_bus[8]);
    pcm_bus[PCM_BUS_3V3].connect(pdm_bus[9]);
}

uint32_t Eps::get_command_param(const I2CData& data)
{
    uint32_t param = 0;

    // copy parameter bytes (skip first command byte)
    I2CData param_data(data.begin() + 1, data.end());

    // swap byte order
    if(swap.in)
    {
        std::reverse(param_data.begin(), param_data.end());
    }

    // get parameter value
    for(int i = 0; i < param_data.size(); i++)
    {
        param |= (param_data[i] << (i * 8));
    }

    return param;
}

