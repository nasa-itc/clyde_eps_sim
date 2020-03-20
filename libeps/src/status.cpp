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

#include "status.hpp"

using namespace itc::eps;

Status::Status() :
    checksum(CHECKSUM),
    status(),
    manual_reset(false),
    reset_counts(),
    last_error(ERROR_NONE)
{
}

Status::Status(uint16_t status) :
    checksum(CHECKSUM),
    status(status),
    manual_reset(false),
    reset_counts(),
    last_error(ERROR_NONE)
{
}

Status::~Status()
{
}

uint16_t Status::get_checksum() const
{
    return checksum;
}

uint16_t Status::get_status() const
{
    return static_cast<uint16_t>(status.to_ulong());
}

void Status::set_status(uint16_t status)
{
    this->status = status;
}

bool Status::is_set(BoardStatus bit) const
{
    return status[bit];
}

void Status::set(BoardStatus bit, bool state)
{
    status.set(bit, state);
    if(state)
    {
        switch(bit)
        {
            case STATUS_INVALID_CMD:
                last_error = ERROR_INVALID_CMD;
                break;
            case STATUS_INVALID_DATA:
                last_error = ERROR_INVALID_DATA;
                break;
            case STATUS_INVALID_CHANNEL:
                last_error = ERROR_INVALID_CHANNEL;
                break;
            case STATUS_EEPROM_ERROR:
                last_error = ERROR_EEPROM_READ;
                break;
            default:
                break;
        }
    }
}

bool Status::is_set(ResetType type) const
{
    if(type == RESET_MANUAL)
    {
        return manual_reset;
    }
    else
    {
        return status[type];
    }
}

void Status::set(ResetType type)
{
    if(type == RESET_MANUAL)
    {
        manual_reset = true;
    }
    else
    {
        // only one active reset state
        clear_resets();
        status.set(type, true);
    }

    // TODO what reset types are considered error?
    last_error = ERROR_RESET;
    reset_counts[type]++;
}

void Status::clear_all()
{
    status.reset();
}

void Status::clear(ResetType type)
{
    if(type == RESET_MANUAL)
    {
        manual_reset = false;
    }
    else
    {
        status.reset(type);
    }
}

void Status::clear_resets()
{
    manual_reset = false;
    status.reset(RESET_WDT);
    status.reset(RESET_POWER_ON);
    status.reset(RESET_BROWN_OUT);
}

uint8_t Status::get_num_resets(ResetType type) const
{
    uint8_t num_resets = 0;
    ResetCount::const_iterator it = reset_counts.find(type);
    if(it != reset_counts.end())
    {
        num_resets = it->second;
    }
    return num_resets;
}

ErrorCode Status::get_last_error() const
{
    return last_error;
}

