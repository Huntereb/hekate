/*
* Copyright (c) 2018 naehrwert
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU General Public License,
* version 2, as published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gpio.h"
#include "t210.h"

static const u16 _gpio_cnf[31] = {
	0x000, 0x004, 0x008, 0x00C,
	0x100, 0x104, 0x108, 0x10C,
	0x200, 0x204, 0x208, 0x20C,
	0x300, 0x304, 0x308, 0x30C,
	0x400, 0x404, 0x408, 0x40C,
	0x500, 0x504, 0x508, 0x50C,
	0x600, 0x604, 0x608, 0x60C,
	0x700, 0x704, 0x708
};

static const u16 _gpio_oe[31] = {
	0x010, 0x014, 0x018, 0x01C,
	0x110, 0x114, 0x118, 0x11C,
	0x210, 0x214, 0x218, 0x21C,
	0x310, 0x314, 0x318, 0x31C,
	0x410, 0x414, 0x418, 0x41C,
	0x510, 0x514, 0x518, 0x51C,
	0x610, 0x614, 0x618, 0x61C,
	0x710, 0x714, 0x718
};

static const u16 _gpio_out[31] = {
	0x020, 0x024, 0x028, 0x02C,
	0x120, 0x124, 0x128, 0x12C,
	0x220, 0x224, 0x228, 0x22C,
	0x320, 0x324, 0x328, 0x32C,
	0x420, 0x424, 0x428, 0x42C,
	0x520, 0x524, 0x528, 0x52C,
	0x620, 0x624, 0x628, 0x62C,
	0x720, 0x724, 0x728
};

static const u16 _gpio_in[31] = {
	0x030, 0x034, 0x038, 0x03C,
	0x130, 0x134, 0x138, 0x13C,
	0x230, 0x234, 0x238, 0x23C,
	0x330, 0x334, 0x338, 0x33C,
	0x430, 0x434, 0x438, 0x43C,
	0x530, 0x534, 0x538, 0x53C,
	0x630, 0x634, 0x638, 0x63C,
	0x730, 0x734, 0x738
};

void gpio_config(u32 port, u32 pins, int mode)
{
	if (mode)
		GPIO(_gpio_cnf[port]) |= pins;
	else
		GPIO(_gpio_cnf[port]) &= ~pins;
	(void)GPIO(_gpio_cnf[port]);
}

void gpio_output_enable(u32 port, u32 pins, int enable)
{
	if (enable)
		GPIO(_gpio_oe[port]) |= pins;
	else
		GPIO(_gpio_oe[port]) &= ~pins;
	(void)GPIO(_gpio_oe[port]);
}

void gpio_write(u32 port, u32 pins, int high)
{
	if (high)
		GPIO(_gpio_out[port]) |= pins;
	else
		GPIO(_gpio_out[port]) &= ~pins;
	(void)GPIO(_gpio_out[port]);
}

int gpio_read(u32 port, u32 pins)
{
	return (GPIO(_gpio_in[port]) & pins) ? 1 : 0;
}
