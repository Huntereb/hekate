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

#include <stdarg.h>
#include <string.h>
#include "gfx.h"

static const u8 _gfx_font[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x18, 0x18, 0x00, 0x0C, 0x00,
	0x00, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0xFF, 0x66, 0xFF, 0x66, 0x66,
	0x00, 0x18, 0x7C, 0x06, 0x3C, 0x60, 0x3E, 0x18, 0x10, 0x46, 0x66, 0x30, 0x18, 0x0C, 0x66, 0x62,
	0x00, 0x3C, 0x66, 0x3C, 0x1C, 0xE6, 0x66, 0xFC, 0x00, 0x18, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x30, 0x18, 0x0C, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x0C, 0x18, 0x30, 0x30, 0x18, 0x0C, 0x00,
	0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x40, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00,
	0x00, 0x3C, 0x66, 0x76, 0x6E, 0x66, 0x3C, 0x00, 0x00, 0x18, 0x1C, 0x18, 0x18, 0x18, 0x7E, 0x00,
	0x00, 0x3C, 0x62, 0x30, 0x0C, 0x06, 0x7E, 0x00, 0x00, 0x3C, 0x62, 0x38, 0x60, 0x66, 0x3C, 0x00,
	0x00, 0x6C, 0x6C, 0x66, 0xFE, 0x60, 0x60, 0x00, 0x00, 0x7E, 0x06, 0x7E, 0x60, 0x66, 0x3C, 0x00,
	0x00, 0x3C, 0x06, 0x3E, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x7E, 0x30, 0x30, 0x18, 0x18, 0x18, 0x00,
	0x00, 0x3C, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x3C, 0x66, 0x7C, 0x60, 0x66, 0x3C, 0x00,
	0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x18, 0x0C, 0x00,
	0x00, 0x70, 0x1C, 0x06, 0x06, 0x1C, 0x70, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x3E, 0x00, 0x00, 0x00,
	0x00, 0x0E, 0x38, 0x60, 0x60, 0x38, 0x0E, 0x00, 0x00, 0x3C, 0x66, 0x30, 0x18, 0x00, 0x18, 0x00,
	0x00, 0x3C, 0x66, 0x76, 0x76, 0x06, 0x46, 0x3C, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00,
	0x00, 0x3E, 0x66, 0x3E, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x3C, 0x66, 0x06, 0x06, 0x66, 0x3C, 0x00,
	0x00, 0x1E, 0x36, 0x66, 0x66, 0x36, 0x1E, 0x00, 0x00, 0x7E, 0x06, 0x1E, 0x06, 0x06, 0x7E, 0x00,
	0x00, 0x3E, 0x06, 0x1E, 0x06, 0x06, 0x06, 0x00, 0x00, 0x3C, 0x66, 0x06, 0x76, 0x66, 0x3C, 0x00,
	0x00, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, 0x00, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00,
	0x00, 0x78, 0x30, 0x30, 0x30, 0x36, 0x1C, 0x00, 0x00, 0x66, 0x36, 0x1E, 0x1E, 0x36, 0x66, 0x00,
	0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x7E, 0x00, 0x00, 0x46, 0x6E, 0x7E, 0x56, 0x46, 0x46, 0x00,
	0x00, 0x66, 0x6E, 0x7E, 0x76, 0x66, 0x66, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00,
	0x00, 0x3E, 0x66, 0x3E, 0x06, 0x06, 0x06, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x70, 0x00,
	0x00, 0x3E, 0x66, 0x3E, 0x1E, 0x36, 0x66, 0x00, 0x00, 0x3C, 0x66, 0x0C, 0x30, 0x66, 0x3C, 0x00,
	0x00, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00,
	0x00, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, 0x00, 0x46, 0x46, 0x56, 0x7E, 0x6E, 0x46, 0x00,
	0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00, 0x00, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00,
	0x00, 0x7E, 0x30, 0x18, 0x0C, 0x06, 0x7E, 0x00, 0x00, 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C,
	0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00, 0x00, 0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C,
	0x00, 0x18, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E,
	0x00, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x60, 0x7C, 0x66, 0x7C, 0x00,
	0x00, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x3C, 0x06, 0x06, 0x06, 0x3C, 0x00,
	0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x7C, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x7E, 0x06, 0x3C, 0x00,
	0x00, 0x38, 0x0C, 0x3E, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x7C, 0x66, 0x7C, 0x40, 0x3C, 0x00,
	0x00, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x00, 0x00, 0x18, 0x00, 0x1C, 0x18, 0x18, 0x3C, 0x00,
	0x00, 0x30, 0x00, 0x30, 0x30, 0x30, 0x1E, 0x00, 0x00, 0x06, 0x06, 0x36, 0x1E, 0x36, 0x66, 0x00,
	0x00, 0x1C, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x66, 0xFE, 0xFE, 0xD6, 0xC6, 0x00,
	0x00, 0x00, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00,
	0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x00,
	0x00, 0x00, 0x3E, 0x66, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x7C, 0x06, 0x3C, 0x60, 0x3E, 0x00,
	0x00, 0x18, 0x7E, 0x18, 0x18, 0x18, 0x70, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x00,
	0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, 0x00, 0x00, 0xC6, 0xD6, 0xFE, 0x7C, 0x6C, 0x00,
	0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x66, 0x66, 0x7C, 0x60, 0x3C, 0x00,
	0x00, 0x00, 0x7E, 0x30, 0x18, 0x0C, 0x7E, 0x00, 0x00, 0x00, 0x18, 0x08, 0x08, 0x04, 0x08, 0x08,
	0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x0C, 0x08, 0x08, 0x10, 0x08, 0x08,
	0x00, 0x00, 0x00, 0x4c, 0x32, 0x00, 0x00, 0x00
};

void gfx_init_ctxt(gfx_ctxt_t *ctxt, u32 *fb, u32 width, u32 height, u32 stride)
{
	ctxt->fb = fb;
	ctxt->width = width;
	ctxt->height = height;
	ctxt->stride = stride;
}

void gfx_clear_grey(gfx_ctxt_t *ctxt, u8 color)
{
	memset(ctxt->fb, color, 0x400000);
}

void gfx_clear_color(gfx_ctxt_t *ctxt, u32 color)
{
	for (u32 i = 0; i < ctxt->height * ctxt->stride; i++)
		ctxt->fb[i] = color;
}

void gfx_con_init(gfx_con_t *con, gfx_ctxt_t *ctxt)
{
	con->gfx_ctxt = ctxt;
	con->x = 0;
	con->y = 0;
	con->fgcol = 0xFFCCCCCC;
	con->fillbg = 0;
	con->bgcol = 0xFF1B1B1B;
}

void gfx_con_setcol(gfx_con_t *con, u32 fgcol, int fillbg, u32 bgcol)
{
	con->fgcol = fgcol;
	con->fillbg = fillbg;
	con->bgcol = bgcol;
}

void gfx_con_getpos(gfx_con_t *con, u32 *x, u32 *y)
{
	*x = con->x;
	*y = con->y;
}

void gfx_con_setpos(gfx_con_t *con, u32 x, u32 y)
{
	con->x = x;
	con->y = y;
}

void gfx_putc(gfx_con_t *con, char c)
{
	if (c >= 32 && c <= 126)
	{
		u8 *cbuf = (u8 *)&_gfx_font[8 * (c - 32)];
		u32 *fb = con->gfx_ctxt->fb + con->x + con->y * con->gfx_ctxt->stride;
		for (u32 i = 0; i < 8; i++)
		{
			u8 v = *cbuf++;
			for (u32 j = 0; j < 8; j++)
			{
				if (v & 1)
					*fb = con->fgcol;
				else if (con->fillbg)
					*fb = con->bgcol;
				v >>= 1;
				fb++;
			}
			fb += con->gfx_ctxt->stride - 8;
		}
		con->x += 8;
	}
	else if (c == '\n')
	{
		con->x = 0;
		con->y += 8;
		if (con->y > con->gfx_ctxt->height - 8)
			con->y = 0;
	}
}

void gfx_puts(gfx_con_t *con, const char *s)
{
	if (!s)
		return;

	for (; *s; s++)
		gfx_putc(con, *s);
}

static void _gfx_putn(gfx_con_t *con, u32 v, int base, char fill, int fcnt)
{
	char buf[65];
	static const char digits[] = "0123456789ABCDEFghijklmnopqrstuvwxyz";
	char *p;
	int c = fcnt;

	if (base > 36)
		return;

	p = buf + 64;
	*p = 0;
	do
	{
		c--;
		*--p = digits[v % base];
		v /= base;
	} while (v);

	if (fill != 0)
	{
		while (c > 0)
		{
			*--p = fill;
			c--;
		}
	}

	gfx_puts(con, p);
}

void gfx_printf(gfx_con_t *con, const char *fmt, ...)
{
	va_list ap;
	int fill, fcnt;

	va_start(ap, fmt);
	while(*fmt)
	{
		if(*fmt == '%')
		{
			fmt++;
			fill = 0;
			fcnt = 0;
			if ((*fmt >= '0' && *fmt <= '9') || *fmt == ' ')
			{
				fcnt = *fmt;
				fmt++;
				if (*fmt >= '0' && *fmt <= '9')
				{
					fill = fcnt;
					fcnt = *fmt - '0';
					fmt++;
				}
				else
				{
					fill = ' ';
					fcnt -= '0';
				}
			}
			switch(*fmt)
			{
			case 'c':
				gfx_putc(con, va_arg(ap, u32));
				break;
			case 's':
				gfx_puts(con, va_arg(ap, char *));
				break;
			case 'd':
				_gfx_putn(con, va_arg(ap, u32), 10, fill, fcnt);
				break;
			case 'x':
			case 'X':
				_gfx_putn(con, va_arg(ap, u32), 16, fill, fcnt);
				break;
			case 'k':
				con->fgcol = va_arg(ap, u32);
				break;
			case 'K':
				con->bgcol = va_arg(ap, u32);
				con->fillbg = fcnt;
				break;
			case '%':
				gfx_putc(con, '%');
				break;
			case '\0':
				goto out;
			default:
				gfx_putc(con, '%');
				gfx_putc(con, *fmt);
				break;
			}
		}
		else
			gfx_putc(con, *fmt);
		fmt++;
	}

	out:
	va_end(ap);
}

void gfx_hexdump(gfx_con_t *con, u32 base, const u8 *buf, u32 len)
{
	for(u32 i = 0; i < len; i++)
	{
		if(i % 0x10 == 0)
		{
			if(i != 0)
			{
				gfx_puts(con, "| ");
				for(u32 j = 0; j < 0x10; j++)
				{
					u8 c = buf[i - 0x10 + j];
					if(c >= 32 && c <= 126)
						gfx_putc(con, c);
					else
						gfx_putc(con, '.');
				}
				gfx_putc(con, '\n');
			}
			gfx_printf(con, "%08x: ", base + i);
		}
		gfx_printf(con, "%02x ", buf[i]);
		if (i == len - 1)
		{
			int ln = len % 0x10 != 0;
			u32 k = 0x10 - 1;
			if (ln)
			{
				k = (len & 0xF) - 1;
				for (u32 j = 0; j < 0x10 - k; j++)
					gfx_puts(con, "   ");
			}
			gfx_puts(con, "| ");
			for(u32 j = 0; j < (ln ? k : k + 1); j++)
			{
				u8 c = buf[i - k + j];
				if(c >= 32 && c <= 126)
					gfx_putc(con, c);
				else
					gfx_putc(con, '.');
			}
			gfx_putc(con, '\n');
		}
	}
	gfx_putc(con, '\n');
}

static int abs(int x)
{
	if (x < 0)
		return -x;
	return x;
}

void gfx_set_pixel(gfx_ctxt_t *ctxt, u32 x, u32 y, u32 color)
{
	ctxt->fb[x + y * ctxt->stride] = color;
}

void gfx_line(gfx_ctxt_t *ctxt, int x0, int y0, int x1, int y1, u32 color)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1; 
	int err = (dx > dy ? dx : -dy) / 2, e2;

	while (1)
	{
		gfx_set_pixel(ctxt, x0, y0, color);
		if (x0 == x1 && y0 == y1)
			break;
		e2 = err;
		if (e2 >-dx)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy)
		{
			err += dx;
			y0 += sy;
		}
	}
}
