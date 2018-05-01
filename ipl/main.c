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

#include <string.h>

#include "clock.h"
#include "uart.h"
#include "i2c.h"
#include "sdram.h"
#include "di.h"
#include "mc.h"
#include "t210.h"
#include "pmc.h"
#include "pinmux.h"
#include "fuse.h"
#include "util.h"
#include "gfx.h"
#include "btn.h"
#include "tsec.h"
#include "kfuse.h"
#include "max77620.h"
#include "max7762x.h"
#include "gpio.h"
#include "sdmmc.h"
#include "ff.h"
#include "tui.h"
#include "heap.h"
#include "list.h"
#include "nx_emmc.h"
#include "se.h"
#include "se_t210.h"
#include "hos.h"
#include "pkg1.h"

void panic(u32 val)
{
	//Set panic code.
	PMC(APBDEV_PMC_SCRATCH200) = val;
	//PMC(APBDEV_PMC_CRYPTO_OP) = 1; //Disable SE.
	TMR(0x18C) = 0xC45A;
	TMR(0x80) = 0xC0000000;
	TMR(0x180) = 0x8019;
	TMR(0x188) = 1;
	while (1)
		;
}

void config_oscillators()
{
	CLOCK(CLK_RST_CONTROLLER_SPARE_REG0) = CLOCK(CLK_RST_CONTROLLER_SPARE_REG0) & 0xFFFFFFF3 | 4;
	SYSCTR0(SYSCTR0_CNTFID0) = 19200000;
	TMR(0x14) = 0x45F;
	CLOCK(CLK_RST_CONTROLLER_OSC_CTRL) = 0x50000071;
	PMC(APBDEV_PMC_OSC_EDPD_OVER) = PMC(APBDEV_PMC_OSC_EDPD_OVER) & 0xFFFFFF81 | 0xE;
	PMC(APBDEV_PMC_OSC_EDPD_OVER) = PMC(APBDEV_PMC_OSC_EDPD_OVER) & 0xFFBFFFFF | 0x400000;
	PMC(APBDEV_PMC_CNTRL2) = PMC(APBDEV_PMC_CNTRL2) & 0xFFFFEFFF | 0x1000;
	PMC(APBDEV_PMC_SCRATCH188) = PMC(APBDEV_PMC_SCRATCH188) & 0xFCFFFFFF | 0x2000000;
	CLOCK(CLK_RST_CONTROLLER_CLK_SYSTEM_RATE) = 0x10;
	CLOCK(CLK_RST_CONTROLLER_PLLMB_BASE) &= 0xBFFFFFFF;
	PMC(APBDEV_PMC_TSC_MULT) = PMC(APBDEV_PMC_TSC_MULT) & 0xFFFF0000 | 0x249F; //0x249F = 19200000 * (16 / 32.768 kHz)
	CLOCK(CLK_RST_CONTROLLER_SCLK_BURST_POLICY) = 0x20004444;
	CLOCK(CLK_RST_CONTROLLER_SUPER_SCLK_DIVIDER) = 0x80000000;
	CLOCK(CLK_RST_CONTROLLER_CLK_SYSTEM_RATE) = 2;
}

void config_gpios()
{
	PINMUX_AUX(PINMUX_AUX_UART2_TX) = 0;
	PINMUX_AUX(PINMUX_AUX_UART3_TX) = 0;

	PINMUX_AUX(PINMUX_AUX_GPIO_PE6) = 0x40;
	PINMUX_AUX(PINMUX_AUX_GPIO_PH6) = 0x40;

	gpio_config(GPIO_PORT_G, GPIO_PIN_0, GPIO_MODE_GPIO);
	gpio_config(GPIO_PORT_D, GPIO_PIN_1, GPIO_MODE_GPIO);
	gpio_config(GPIO_PORT_E, GPIO_PIN_6, GPIO_MODE_GPIO);
	gpio_config(GPIO_PORT_H, GPIO_PIN_6, GPIO_MODE_GPIO);
	gpio_output_enable(GPIO_PORT_G, GPIO_PIN_0, GPIO_OUTPUT_DISABLE);
	gpio_output_enable(GPIO_PORT_D, GPIO_PIN_1, GPIO_OUTPUT_DISABLE);
	gpio_output_enable(GPIO_PORT_E, GPIO_PIN_6, GPIO_OUTPUT_DISABLE);
	gpio_output_enable(GPIO_PORT_H, GPIO_PIN_6, GPIO_OUTPUT_DISABLE);

	pinmux_config_i2c(I2C_1);
	pinmux_config_i2c(I2C_5);
	pinmux_config_uart(UART_A);

	//Configure volume up/down as inputs.
	gpio_config(GPIO_PORT_X, GPIO_PIN_6, GPIO_MODE_GPIO);
	gpio_config(GPIO_PORT_X, GPIO_PIN_7, GPIO_MODE_GPIO);
	gpio_output_enable(GPIO_PORT_X, GPIO_PIN_6, GPIO_OUTPUT_DISABLE);
	gpio_output_enable(GPIO_PORT_X, GPIO_PIN_7, GPIO_OUTPUT_DISABLE);
}

void config_pmc_scratch()
{
	PMC(APBDEV_PMC_SCRATCH20) &= 0xFFF3FFFF;
	PMC(APBDEV_PMC_SCRATCH190) &= 0xFFFFFFFE;
	PMC(APBDEV_PMC_SECURE_SCRATCH21) |= 0x10;
}

void mbist_workaround()
{
	CLOCK(0x410) = (CLOCK(0x410) | 0x8000) & 0xFFFFBFFF;
	CLOCK(0xD0) |= 0x40800000u;
	CLOCK(0x2AC) = 0x40;
	CLOCK(0x294) = 0x40000;
	CLOCK(0x304) = 0x18000000;
	sleep(2);

	I2S(0x0A0) |= 0x400;
	I2S(0x088) &= 0xFFFFFFFE;
	I2S(0x1A0) |= 0x400;
	I2S(0x188) &= 0xFFFFFFFE;
	I2S(0x2A0) |= 0x400;
	I2S(0x288) &= 0xFFFFFFFE;
	I2S(0x3A0) |= 0x400;
	I2S(0x388) &= 0xFFFFFFFE;
	I2S(0x4A0) |= 0x400;
	I2S(0x488) &= 0xFFFFFFFE;
	DISPLAY_A(0xCF8) |= 4;
	VIC(0x8C) = 0xFFFFFFFF;
	sleep(2);

	CLOCK(0x2A8) = 0x40;
	CLOCK(0x300) = 0x18000000;
	CLOCK(0x290) = 0x40000;
	CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_H) = 0xC0;
	CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_L) = 0x80000130;
	CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_U) = 0x1F00200;
	CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_V) = 0x80400808;
	CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_W) = 0x402000FC;
	CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_X) = 0x23000780;
	CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_Y) = 0x300;
	CLOCK(0xF8) = 0;
	CLOCK(0xFC) = 0;
	CLOCK(0x3A0) = 0;
	CLOCK(0x3A4) = 0;
	CLOCK(0x554) = 0;
	CLOCK(0xD0) &= 0x1F7FFFFF;
	CLOCK(0x410) &= 0xFFFF3FFF;
	CLOCK(0x148) = CLOCK(0x148) & 0x1FFFFFFF | 0x80000000;
	CLOCK(0x180) = CLOCK(0x180) & 0x1FFFFFFF | 0x80000000;
	CLOCK(0x6A0) = CLOCK(0x6A0) & 0x1FFFFFFF | 0x80000000;
}

void config_se_brom()
{
	//Bootrom part we skipped.
	u32 sbk[4] = { FUSE(0x1A4), FUSE(0x1A8), FUSE(0x1AC), FUSE(0x1B0) };
	se_aes_key_set(14, sbk, 0x10);
	//Lock SBK from being read.
	SE(SE_KEY_TABLE_ACCESS_REG_OFFSET + 14 * 4) = 0x7E;
	//This memset needs to happen here, else TZRAM will behave weirdly later on.
	memset((void *)0x7C010000, 0, 0x10000);
	PMC(APBDEV_PMC_CRYPTO_OP) = 0;
	SE(SE_INT_STATUS_REG_OFFSET) = 0x1F;
	//Lock SSK (although it's not set and unused anyways).
	SE(SE_KEY_TABLE_ACCESS_REG_OFFSET + 15 * 4) = 0x7E;
}

void config_hw()
{
	//Bootrom stuff we skipped by going thru rcm.
	config_se_brom();
	//FUSE(FUSE_PRIVATEKEYDISABLE) = 0x11;
	SYSREG(0x110) &= 0xFFFFFF9F;
	PMC(0x244) = ((PMC(0x244) >> 1) << 1) & 0xFFFFFFFD;

	mbist_workaround();
	clock_enable_se();

	//Enable fuse clock.
	clock_enable_fuse(1);
	//Disable fuse programming.
	fuse_disable_program();

	mc_enable();

	config_oscillators();
	APB_MISC(0x40) = 0;
	config_gpios();

	//clock_enable_uart(UART_C);
	//uart_init(UART_C, 115200);

	clock_enable_cl_dvfs();

	clock_enable_i2c(I2C_1);
	clock_enable_i2c(I2C_5);

	static const clock_t clock_unk1 = { 0x358, 0x360, 0x42C, 0x1F, 0, 0 };
	static const clock_t clock_unk2 = { 0x358, 0x360, 0, 0x1E, 0, 0 };
	clock_enable(&clock_unk1);
	clock_enable(&clock_unk2);

	i2c_init(I2C_1);
	i2c_init(I2C_5);

	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_CNFGBBC, 0x40);
	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_ONOFFCNFG1, 0x78);

	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_CFG0, 0x38);
	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_CFG1, 0x3A);
	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_CFG2, 0x38);
	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_LDO4, 0xF);
	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_LDO8, 0xC7);
	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_SD0, 0x4F);
	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_SD1, 0x29);
	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_SD3, 0x1B);

	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_SD0, 42); //42 = (1125000 - 600000) / 12500 -> 1.125V

	config_pmc_scratch();

	CLOCK(CLK_RST_CONTROLLER_SCLK_BURST_POLICY) = CLOCK(CLK_RST_CONTROLLER_SCLK_BURST_POLICY) & 0xFFFF8888 | 0x3333;

	mc_config_carveout();

	sdram_init();
	//TODO: test this with LP0 wakeup.
	sdram_lp0_save_params(sdram_get_params());
}

//TODO: ugly.
gfx_ctxt_t gfx_ctxt;
gfx_con_t gfx_con;

void print_fuseinfo()
{
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	gfx_printf(&gfx_con, "%k(Unlocked) fuse cache:\n\n%k", 0xFFFF9955, 0xFFFFFFFF);
	gfx_hexdump(&gfx_con, 0x7000F900, (u8 *)0x7000F900, 0x2FC);

	sleep(100000);
	btn_wait();
}

void print_kfuseinfo()
{
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	gfx_printf(&gfx_con, "%kKFuse contents:\n\n%k", 0xFFFF9955, 0xFFFFFFFF);
	u32 buf[KFUSE_NUM_WORDS];
	if (!kfuse_read(buf))
		gfx_printf(&gfx_con, "%kCRC fail.\n", 0xFF0000FF);
	else
		gfx_hexdump(&gfx_con, 0, (u8 *)buf, KFUSE_NUM_WORDS * 4);

	sleep(100000);
	btn_wait();
}

void print_tsec_key()
{
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	sdmmc_storage_t storage;
	sdmmc_t sdmmc;

	sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4);

	//Read package1.
	u8 *pkg1 = (u8 *)malloc(0x40000);
	sdmmc_storage_set_mmc_partition(&storage, 1);
	sdmmc_storage_read(&storage, 0x100000 / NX_EMMC_BLOCKSIZE, 0x40000 / NX_EMMC_BLOCKSIZE, pkg1);
	const pkg1_id_t *pkg1_id = pkg1_identify(pkg1);
	if (!pkg1_id)
	{
		gfx_printf(&gfx_con, "%kCould not identify package 1 version to read TSEC firmware (= '%s').%k\n", 0xFF0000FF, (char *)pkg1 + 0x10, 0xFFFFFFFF);
		goto out;
	}

	for(u32 i = 1; i <= 3; i++)
	{
		u8 key[0x10];
		int res = tsec_query(key, i, pkg1 + pkg1_id->tsec_off);

		gfx_printf(&gfx_con, "%kTSEC key %d: %k", 0xFFFF9955, i, 0xFFFFFFFF);
		if (res >= 0)
		{
			for (u32 i = 0; i < 0x10; i++)
				gfx_printf(&gfx_con, "%02X", key[i]);
		}
		else
			gfx_printf(&gfx_con, "%kERROR %X", 0xFF0000FF, res);
		gfx_putc(&gfx_con, '\n');
	}

out:;
	free(pkg1);
	sdmmc_storage_end(&storage);
	sleep(100000);
	btn_wait();
}

void reboot_normal()
{
	panic(0x21); //Bypass fuse programming in package1.
}

void reboot_rcm()
{
	PMC(APBDEV_PMC_SCRATCH0) = 2; //Reboot into rcm.
	PMC(0) |= 0x10;
	while (1)
		sleep(1);
}

void power_off()
{
	//TODO: we should probably make sure all regulators are powered off properly.
	i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_ONOFFCNFG1, MAX77620_ONOFFCNFG1_PWR_OFF);
}

//TODO: ugly.
sdmmc_t sd_sdmmc;
sdmmc_storage_t sd_storage;
FATFS sd_fs;
int sd_mounted;

int sd_mount()
{
	if (sd_mounted)
		return 1;

	if (sdmmc_storage_init_sd(&sd_storage, &sd_sdmmc, SDMMC_1, SDMMC_BUS_WIDTH_4, 11) &&
		f_mount(&sd_fs, "", 1) == FR_OK)
	{
		sd_mounted = 1;
		return 1;
	}

	return 0;
}

void *sd_file_read(char *path)
{
	FIL fp;
	if (f_open(&fp, path, FA_READ) != FR_OK)
		return NULL;

	u32 size = f_size(&fp);
	void *buf = malloc(size);

	u8 *ptr = buf;
	while (size > 0)
	{
		u32 rsize = MIN(size, 512);
		if (f_read(&fp, ptr, rsize, NULL) != FR_OK)
		{
			free(buf);
			return NULL;
		}

		ptr += rsize;
		size -= rsize;
	}

	f_close(&fp);

	return buf;
}

int dump_emmc_part(char *sd_path, sdmmc_storage_t *storage, emmc_part_t *part)
{
	FIL fp;
	if (f_open(&fp, sd_path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
		return 0;

	u8 *buf = (u8 *)malloc(NX_EMMC_BLOCKSIZE * 512);

	u32 total = part->lba_end - part->lba_start + 1;
	u32 lba_curr = part->lba_start;
	while(total > 0)
	{
		u32 num = MIN(total, 512);

		if(!sdmmc_storage_read(storage, lba_curr, num, buf))
		{
			gfx_printf(&gfx_con, "%kError reading %d blocks @ LBA %08X%k\n",
				0xFF0000FF, num, lba_curr, 0xFFFFFFFF);
			goto out;
		}
		f_write(&fp, buf, NX_EMMC_BLOCKSIZE * num, NULL);
		u32 pct = ((lba_curr - part->lba_start) * 100) / (part->lba_end - part->lba_start);
		tui_pbar(&gfx_con, 0, gfx_con.y, pct);

		lba_curr += num;
		total -= num;
	}
	tui_pbar(&gfx_con, 0, gfx_con.y, 100);

out:;
	free(buf);
	f_close(&fp);
	return 1;
}

void dump_emmc()
{
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	if (!sd_mount())
	{
		gfx_printf(&gfx_con, "%kFailed to mount SD card (make sure that it is inserted).%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}

	sdmmc_storage_t storage;
	sdmmc_t sdmmc;
	if(!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4))
	{
		gfx_printf(&gfx_con, "%kFailed to init eMMC.%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}
	sdmmc_storage_set_mmc_partition(&storage, 0);

	LIST_INIT(gpt);
	nx_emmc_gpt_parse(&gpt, &storage);
	int i = 0;
	LIST_FOREACH_ENTRY(emmc_part_t, part, &gpt, link)
	{
		gfx_printf(&gfx_con, "%02d: %s (%08X-%08X)\n", i++,
			part->name, part->lba_start, part->lba_end);

		//XXX: skip these for now.
		if (//!strcmp(part->name, "SYSTEM") || 
			!strcmp(part->name, "USER"))
		{
			gfx_puts(&gfx_con, "Skipped.\n");
			continue;
		}

		dump_emmc_part(part->name, &storage, part);
		gfx_putc(&gfx_con, '\n');
	}

	sdmmc_storage_end(&storage);

out:;
	sleep(100000);
	btn_wait();
}

void dump_boot0_emmc()
{
	emmc_part_t *part = (emmc_part_t *)malloc(sizeof(emmc_part_t));
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	if (!sd_mount())
	{
		gfx_printf(&gfx_con, "%kFailed to mount SD card (make sure that it is inserted).%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}

	sdmmc_storage_t storage;
	sdmmc_t sdmmc;
	if(!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4))
	{
		gfx_printf(&gfx_con, "%kFailed to init eMMC.%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}
	sdmmc_storage_set_mmc_partition(&storage, 1);

	//LIST_INIT(gpt);
	//nx_emmc_gpt_parse(&gpt, &storage);

	part->name[0] = 0x62;
	part->name[1] = 0x6f;
	part->name[2] = 0x6f;
	part->name[3] = 0x74;
	part->name[4] = 0x5f;
	part->name[6] = 0x0;

	part->name[5] = 0x30;
	part->lba_start = 0x0000;
	part->lba_end   = 0x1FFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	sdmmc_storage_end(&storage);

out:;
	sleep(100000);
	btn_wait();
}

void dump_boot1_emmc()
{
	emmc_part_t *part = (emmc_part_t *)malloc(sizeof(emmc_part_t));
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	if (!sd_mount())
	{
		gfx_printf(&gfx_con, "%kFailed to mount SD card (make sure that it is inserted).%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}

	sdmmc_storage_t storage;
	sdmmc_t sdmmc;
	if(!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4))
	{
		gfx_printf(&gfx_con, "%kFailed to init eMMC.%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}
	sdmmc_storage_set_mmc_partition(&storage, 2);

	//LIST_INIT(gpt);
	//nx_emmc_gpt_parse(&gpt, &storage);

	part->name[0] = 0x62;
	part->name[1] = 0x6f;
	part->name[2] = 0x6f;
	part->name[3] = 0x74;
	part->name[4] = 0x5f;
	part->name[6] = 0x0;

	part->name[5] = 0x31;
	part->lba_start = 0x0000;
	part->lba_end   = 0x1FFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	sdmmc_storage_end(&storage);

out:;
	sleep(100000);
	btn_wait();
}

void dump_raw_emmc()
{
	emmc_part_t *part = (emmc_part_t *)malloc(sizeof(emmc_part_t));
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	if (!sd_mount())
	{
		gfx_printf(&gfx_con, "%kFailed to mount SD card (make sure that it is inserted).%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}

	sdmmc_storage_t storage;
	sdmmc_t sdmmc;
	if(!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4))
	{
		gfx_printf(&gfx_con, "%kFailed to init eMMC.%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}
	sdmmc_storage_set_mmc_partition(&storage, 0);

	//LIST_INIT(gpt);
	//nx_emmc_gpt_parse(&gpt, &storage);

	part->name[0] = 0x70;
	part->name[1] = 0x61;
	part->name[2] = 0x72;
	part->name[3] = 0x74;
	part->name[4] = 0x5f;
	part->name[6] = 0x0;

	part->name[5] = 0x30;
	part->lba_start = 0x000000;
	part->lba_end   = 0x7FFFFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	part->name[5] = 0x31;
	part->lba_start = 0x800000;
	part->lba_end   = 0xFFFFFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	sdmmc_storage_end(&storage);

out:;
	sleep(100000);
	btn_wait();
}

void dump_raw_emmc1()
{
	emmc_part_t *part = (emmc_part_t *)malloc(sizeof(emmc_part_t));
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	if (!sd_mount())
	{
		gfx_printf(&gfx_con, "%kFailed to mount SD card (make sure that it is inserted).%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}

	sdmmc_storage_t storage;
	sdmmc_t sdmmc;
	if(!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4))
	{
		gfx_printf(&gfx_con, "%kFailed to init eMMC.%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}
	sdmmc_storage_set_mmc_partition(&storage, 0);

	//LIST_INIT(gpt);
	//nx_emmc_gpt_parse(&gpt, &storage);

	part->name[0] = 0x70;
	part->name[1] = 0x61;
	part->name[2] = 0x72;
	part->name[3] = 0x74;
	part->name[4] = 0x5f;
	part->name[6] = 0x0;

	part->name[5] = 0x32;
	part->lba_start = 0x1000000;
	part->lba_end   = 0x17FFFFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	part->name[5] = 0x33;
	part->lba_start = 0x1800000;
	part->lba_end   = 0x1FFFFFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	sdmmc_storage_end(&storage);

out:;
	sleep(100000);
	btn_wait();
}

void dump_raw_emmc2()
{
	emmc_part_t *part = (emmc_part_t *)malloc(sizeof(emmc_part_t));
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	if (!sd_mount())
	{
		gfx_printf(&gfx_con, "%kFailed to mount SD card (make sure that it is inserted).%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}

	sdmmc_storage_t storage;
	sdmmc_t sdmmc;
	if(!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4))
	{
		gfx_printf(&gfx_con, "%kFailed to init eMMC.%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}
	sdmmc_storage_set_mmc_partition(&storage, 0);

	//LIST_INIT(gpt);
	//nx_emmc_gpt_parse(&gpt, &storage);

	part->name[0] = 0x70;
	part->name[1] = 0x61;
	part->name[2] = 0x72;
	part->name[3] = 0x74;
	part->name[4] = 0x5f;
	part->name[6] = 0x0;

	part->name[5] = 0x34;
	part->lba_start = 0x2000000;
	part->lba_end   = 0x27FFFFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	part->name[5] = 0x35;
	part->lba_start = 0x2800000;
	part->lba_end   = 0x2FFFFFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	sdmmc_storage_end(&storage);

out:;
	sleep(100000);
	btn_wait();
}

void dump_raw_emmc3()
{
	emmc_part_t *part = (emmc_part_t *)malloc(sizeof(emmc_part_t));
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	if (!sd_mount())
	{
		gfx_printf(&gfx_con, "%kFailed to mount SD card (make sure that it is inserted).%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}

	sdmmc_storage_t storage;
	sdmmc_t sdmmc;
	if(!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4))
	{
		gfx_printf(&gfx_con, "%kFailed to init eMMC.%k\n", 0xFF0000FF, 0xFFFFFFFF);
		goto out;
	}
	sdmmc_storage_set_mmc_partition(&storage, 0);

	//LIST_INIT(gpt);
	//nx_emmc_gpt_parse(&gpt, &storage);

	part->name[0] = 0x70;
	part->name[1] = 0x61;
	part->name[2] = 0x72;
	part->name[3] = 0x74;
	part->name[4] = 0x5f;
	part->name[6] = 0x0;

	part->name[5] = 0x36;
	part->lba_start = 0x3000000;
	part->lba_end   = 0x37FFFFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	part->name[5] = 0x37;
	part->lba_start = 0x3800000;
	part->lba_end   = 0x3A3DFFF;
	gfx_printf(&gfx_con, "%s (%08X-%08X)\n",
	part->name, part->lba_start, part->lba_end);
	dump_emmc_part(part->name, &storage, part);
	gfx_putc(&gfx_con, '\n');

	sdmmc_storage_end(&storage);

out:;
	sleep(100000);
	btn_wait();
}


void launch_firmware()
{
	ini_sec_t *cfg_sec = NULL;
	LIST_INIT(ini_sections);

	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	if (sd_mount())
	{
		if (ini_parse(&ini_sections, "hekate_ipl.ini"))
		{
			//Build configuration menu.
			ment_t *ments = (ment_t *)malloc(sizeof(ment_t) * 16);
			ments[0].type = MENT_BACK;
			ments[0].caption = "Back";
			u32 i = 1;
			LIST_FOREACH_ENTRY(ini_sec_t, ini_sec, &ini_sections, link)
			{
				if (!strcmp(ini_sec->name, "config"))
					continue;
				ments[i].type = MENT_CHOICE;
				ments[i].caption = ini_sec->name;
				ments[i].data = ini_sec;
				i++;
			}
			if (i > 1)
			{
				memset(&ments[i], 0, sizeof(ment_t));
				menu_t menu = {
					ments, "Launch configurations", 0, 0
				};
				cfg_sec = (ini_sec_t *)tui_do_menu(&gfx_con, &menu);
				if (!cfg_sec)
					return;
			}
			else
				gfx_printf(&gfx_con, "%kNo launch configurations found.%k\n", 0xFF0000FF, 0xFFFFFFFF);
			free(ments);
		}
		else
			gfx_printf(&gfx_con, "%kFailed to load 'hekate_ipl.ini'.%k\n", 0xFF0000FF, 0xFFFFFFFF);
	}
	else
		gfx_printf(&gfx_con, "%kFailed to mount SD card (make sure that it is inserted).%k\n", 0xFF0000FF, 0xFFFFFFFF);

	if (!cfg_sec)
		gfx_printf(&gfx_con, "Using default launch configuration.\n");

	if (!hos_launch(cfg_sec))
		gfx_printf(&gfx_con, "%kFailed to launch firmware.%k\n", 0xFF0000FF, 0xFFFFFFFF);

	//TODO: free ini.

out:;
	sleep(200000);
	btn_wait();
}

void about()
{
	static const char octopus[] =
	"hekate (c) 2018 naehrwert, st4rk\n\n"
	"Thanks to: %kderrek, nedwill, plutoo, shuffle2, smea, thexyz, yellows8%k\n\n"
	"Greetings to: fincs, hexkyz, SciresM, Shiny Quagsire, WinterMute\n\n"
	"Open source and free packages used:\n"
	" - FatFs R0.13a (Copyright (C) 2017, ChaN)\n"
	" - bcl-1.2.0 (Copyright (c) 2003-2006 Marcus Geelnard)\n\n"
	"                         %k___\n"
	"                      .-'   `'.\n"
	"                     /         \\\n"
	"                     |         ;\n"
	"                     |         |           ___.--,\n"
	"            _.._     |0) = (0) |    _.---'`__.-( (_.\n"
	"     __.--'`_.. '.__.\\    '--. \\_.-' ,.--'`     `\"\"`\n"
	"    ( ,.--'`   ',__ /./;   ;, '.__.'`    __\n"
	"    _`) )  .---.__.' / |   |\\   \\__..--\"\"  \"\"\"--.,_\n"
	"   `---' .'.''-._.-'`_./  /\\ '.  \\ _.--''````'''--._`-.__.'\n"
	"         | |  .' _.-' |  |  \\  \\  '.               `----`\n"
	"          \\ \\/ .'     \\  \\   '. '-._)\n"
	"           \\/ /        \\  \\    `=.__`'-.\n"
	"           / /\\         `) )    / / `\"\".`\\\n"
	"     , _.-'.'\\ \\        / /    ( (     / /\n"
	"      `--'`   ) )    .-'.'      '.'.  | (\n"
	"             (/`    ( (`          ) )  '-;   %k[switchbrew]%k\n"
	"              `      '-;         (-'%k";

	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_setpos(&gfx_con, 0, 0);

	gfx_printf(&gfx_con, octopus, 0xFFFFCC00, 0xFFFFFFFF, 
		0xFFFFCC00, 0xFFCCFF00, 0xFFFFCC00, 0xFFFFFFFF);

	sleep(1000000);
	btn_wait();
}

ment_t ment_cinfo[] = {
	MDEF_BACK(),
	MDEF_HANDLER("Print fuse info", print_fuseinfo),
	MDEF_HANDLER("Print kfuse info", print_kfuseinfo),
	MDEF_HANDLER("Print TSEC keys", print_tsec_key),
	MDEF_END()
};
menu_t menu_cinfo = {
	ment_cinfo,
	"Console info", 0, 0
};

ment_t ment_rawemmc[] = {
	MDEF_BACK(),
	MDEF_HANDLER("Boot0", dump_boot0_emmc),
	MDEF_HANDLER("Boot1", dump_boot1_emmc),
	MDEF_HANDLER("1st 8GB", dump_raw_emmc),
	MDEF_HANDLER("2nd 8GB", dump_raw_emmc1),
	MDEF_HANDLER("3rd 8GB", dump_raw_emmc2),
	MDEF_HANDLER("4th 8GB", dump_raw_emmc3),
	MDEF_END()
};
menu_t menu_rawemmc = {
	ment_rawemmc,
	"Dump raw eMMC", 0, 0
};

ment_t ment_tools[] = {
	MDEF_BACK(),
	MDEF_HANDLER("Dump eMMC", dump_emmc),
	MDEF_MENU("Dump raw eMMC", &menu_rawemmc),
	MDEF_END()
};
menu_t menu_tools = {
	ment_tools,
	"Tools", 0, 0
};

ment_t ment_top[] = {
	MDEF_HANDLER("Launch firmware", launch_firmware),
	MDEF_MENU("Tools", &menu_tools),
	MDEF_MENU("Console info", &menu_cinfo),
	MDEF_HANDLER("Reboot (normal)", reboot_normal),
	MDEF_HANDLER("Reboot (rcm)", reboot_rcm),
	MDEF_HANDLER("Power off", power_off),
	MDEF_HANDLER("About", about),
	MDEF_END()
};
menu_t menu_top = {
	ment_top,
	"hekate - ipl", 0, 0
};

extern void pivot_stack(u32 stack_top);

void ipl_main()
{
	config_hw();

	//Pivot the stack so we have enough space.
	pivot_stack(0x90010000);

	//Tegra/Horizon configuration goes to 0x80000000+, package2 goes to 0xA9800000, we place our heap in between.
	heap_init(0x90020000);

	//uart_send(UART_C, (u8 *)0x40000000, 0x10000);
	//uart_wait_idle(UART_C, UART_TX_IDLE);

	display_init();
	//display_color_screen(0xAABBCCDD);
	u32 *fb = display_init_framebuffer();
	gfx_init_ctxt(&gfx_ctxt, fb, 720, 1280, 768);
	gfx_clear(&gfx_ctxt, 0xFF000000);
	gfx_con_init(&gfx_con, &gfx_ctxt);

	while (1)
		tui_do_menu(&gfx_con, &menu_top);

	while (1)
		;
}
