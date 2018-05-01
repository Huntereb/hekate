#include "mc.h"
#include "t210.h"
#include "clock.h"
#include "util.h"

void mc_config_tsec_carveout(u32 bom, u32 size1mb, int lock)
{
	MC(MC_SEC_CARVEOUT_BOM) = bom;
	MC(MC_SEC_CARVEOUT_SIZE_MB) = size1mb;
	if (lock)
		MC(MC_SEC_CARVEOUT_REG_CTRL) = 1;
}

void mc_config_carveout()
{
	*(vu32 *)0x8005FFFC = 0xC0EDBBCC;
	MC(MC_VIDEO_PROTECT_GPU_OVERRIDE_0) = 1;
	MC(MC_VIDEO_PROTECT_GPU_OVERRIDE_1) = 0;
	MC(MC_VIDEO_PROTECT_BOM) = 0;
	MC(MC_VIDEO_PROTECT_SIZE_MB) = 0;
	MC(MC_VIDEO_PROTECT_REG_CTRL) = 1;

	//Configure TSEC carveout @ 0x90000000, 1MB.
	//mc_config_tsec_carveout(0x90000000, 1, 0);
	mc_config_tsec_carveout(0, 0, 1);

	MC(MC_MTS_CARVEOUT_BOM) = 0;
	MC(MC_MTS_CARVEOUT_SIZE_MB) = 0;
	MC(MC_MTS_CARVEOUT_ADR_HI) = 0;
	MC(MC_MTS_CARVEOUT_REG_CTRL) = 1;
	MC(MC_SECURITY_CARVEOUT1_BOM) = 0;
	MC(MC_SECURITY_CARVEOUT1_BOM_HI) = 0;
	MC(MC_SECURITY_CARVEOUT1_SIZE_128KB) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_ACCESS2) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_ACCESS4) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_FORCE_INTERNAL_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_FORCE_INTERNAL_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_FORCE_INTERNAL_ACCESS2) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_FORCE_INTERNAL_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT1_CLIENT_FORCE_INTERNAL_ACCESS4) = 0;
	MC(MC_SECURITY_CARVEOUT1_CFG0) = 0x4000006;
	MC(MC_SECURITY_CARVEOUT2_BOM) = 0x80020000;
	MC(MC_SECURITY_CARVEOUT2_BOM_HI) = 0;
	MC(MC_SECURITY_CARVEOUT2_SIZE_128KB) = 2;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_ACCESS2) = 0x3000000;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_ACCESS4) = 0x300;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_FORCE_INTERNAL_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_FORCE_INTERNAL_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_FORCE_INTERNAL_ACCESS2) = 0;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_FORCE_INTERNAL_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT2_CLIENT_FORCE_INTERNAL_ACCESS4) = 0;
	MC(MC_SECURITY_CARVEOUT2_CFG0) = 0x440167E;
	MC(MC_SECURITY_CARVEOUT3_BOM) = 0;
	MC(MC_SECURITY_CARVEOUT3_BOM_HI) = 0;
	MC(MC_SECURITY_CARVEOUT3_SIZE_128KB) = 0;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_ACCESS2) = 0x3000000;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_ACCESS4) = 0x300;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_FORCE_INTERNAL_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_FORCE_INTERNAL_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_FORCE_INTERNAL_ACCESS2) = 0;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_FORCE_INTERNAL_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT3_CLIENT_FORCE_INTERNAL_ACCESS4) = 0;
	MC(MC_SECURITY_CARVEOUT3_CFG0) = 0x4401E7E;
	MC(MC_SECURITY_CARVEOUT4_BOM) = 0;
	MC(MC_SECURITY_CARVEOUT4_BOM_HI) = 0;
	MC(MC_SECURITY_CARVEOUT4_SIZE_128KB) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_ACCESS2) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_ACCESS4) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_FORCE_INTERNAL_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_FORCE_INTERNAL_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_FORCE_INTERNAL_ACCESS2) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_FORCE_INTERNAL_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT4_CLIENT_FORCE_INTERNAL_ACCESS4) = 0;
	MC(MC_SECURITY_CARVEOUT4_CFG0) = 0x8F;
	MC(MC_SECURITY_CARVEOUT5_BOM) = 0;
	MC(MC_SECURITY_CARVEOUT5_BOM_HI) = 0;
	MC(MC_SECURITY_CARVEOUT5_SIZE_128KB) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_ACCESS2) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_ACCESS4) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_FORCE_INTERNAL_ACCESS0) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_FORCE_INTERNAL_ACCESS1) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_FORCE_INTERNAL_ACCESS2) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_FORCE_INTERNAL_ACCESS3) = 0;
	MC(MC_SECURITY_CARVEOUT5_CLIENT_FORCE_INTERNAL_ACCESS4) = 0;
	MC(MC_SECURITY_CARVEOUT5_CFG0) = 0x8F;
}

void mc_enable_ahb_redirect()
{
	CLOCK(0x3A4) = CLOCK(0x3A4) & 0xFFF7FFFF | 0x80000;
	//MC(MC_IRAM_REG_CTRL) &= 0xFFFFFFFE;
	MC(MC_IRAM_BOM) = 0x40000000;
	MC(MC_IRAM_TOM) = 0x4003F000;
}

void mc_disable_ahb_redirect()
{
	MC(MC_IRAM_BOM) = 0xFFFFF000;
	MC(MC_IRAM_TOM) = 0;
	//Disable IRAM_CFG_WRITE_ACCESS (sticky).
	//MC(MC_IRAM_REG_CTRL) = MC(MC_IRAM_REG_CTRL) & 0xFFFFFFFE | 1;
	CLOCK(0x3A4) &= 0xFFF7FFFF;
}

void mc_enable()
{
	CLOCK(CLK_RST_CONTROLLER_CLK_SOURCE_EMC) = CLOCK(CLK_RST_CONTROLLER_CLK_SOURCE_EMC) & 0x1FFFFFFF | 0x40000000;
	//Enable MIPI CAL clock.
	CLOCK(CLK_RST_CONTROLLER_CLK_ENB_H_SET) = CLOCK(CLK_RST_CONTROLLER_CLK_ENB_H_SET) & 0xFDFFFFFF | 0x2000000;
	//Enable MC clock.
	CLOCK(CLK_RST_CONTROLLER_CLK_ENB_H_SET) = CLOCK(CLK_RST_CONTROLLER_CLK_ENB_H_SET) & 0xFFFFFFFE | 1;
	//Enable EMC DLL clock.
	CLOCK(CLK_RST_CONTROLLER_CLK_ENB_X_SET) = CLOCK(CLK_RST_CONTROLLER_CLK_ENB_X_SET) & 0xFFFFBFFF | 0x4000;
	CLOCK(CLK_RST_CONTROLLER_RST_DEV_H_SET) = 0x2000001; //Clear EMC and MC reset.
	sleep(5);

	//#ifdef CONFIG_ENABLE_AHB_REDIRECT
	mc_disable_ahb_redirect();
	//mc_enable_ahb_redirect();
	//#endif
}
