/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __EXYNOS_MIPI_DSI_H_
#define __EXYNOS_MIPI_DSI_H_

/* Register definitions */
/* STATUS */
#define DSI_STATUS (0x0)
#define DSI_STATUS_STOPSTATE(x) ((x) << 0)
#define DSI_STATUS_STOPSTATE_CLK (1 << 8)
#define DSI_STATUS_TX_READY_HS_CLK (1 << 10)
#define DSI_STATUS_PLL_STABLE (1 << 31)

/* SWRST */
#define DSI_SWRST  (0x4)
#define DSI_SWRST_SWRST (1 << 0)
#define DSI_SWRST_FUNCRST (1 << 16)

/* CLKCTRL */
#define DSI_CLKCTRL (0x8)
#define DSI_CLKCTRL_HSCLK_EN (1 << 31)
#define DSI_CLKCTRL_ESCCLK_EN (1 << 28)
#define DSI_CLKCTRL_BYTECLK_EN (1 << 24)
#define DSI_CLKCTRL_LANE_ESCCLK(x) ((x) << 19)
#define DSI_CLKCTRL_ESC_PRESCALER_MASK (0xffff)

/* TIMEOUT */
#define DSI_TIMEOUT (0xc)
#define DSI_TIMEOUT_LPDR(x) ((x) & 0xffff)
#define DSI_TIMEOUT_BTA(x) ((x) << 16)

/* CONFIG */
#define DSI_CONFIG (0x10)
#define DSI_CONFIG_AUTOFLUSH(x) (!!(x) << 29)
#define DSI_CONFIG_EOT_PACKET(x) (!!(x) << 28)
#define DSI_CONFIG_BURST_MODE(x) (!!(x) << 26)
#define DSI_CONFIG_VIDEO_MODE(x) (!!(x) << 25)
#define DSI_CONFIG_AUTO_MODE(x) (!!(x) << 24)
#define DSI_CONFIG_HSE_MODE(x) (!!(x) << 23)
#define DSI_CONFIG_HFP_MODE(x) (!!(x) << 22)
#define DSI_CONFIG_HBP_MODE(x) (!!(x) << 21)
#define DSI_CONFIG_HSA_MODE(x) (!!(x) << 20)
#define DSI_CONFIG_MAINVC(x) ((x) << 18)
#define DSI_CONFIG_MAINPIX(x) ((x) << 12)
#define DSI_CONFIG_NUM_LANES(x) ((x) << 5)
#define DSI_CONFIG_LANE_ENABLE(x) ((x) << 1)
#define DSI_CONFIG_CLOCK_LANE (1 << 0)

/* ESCMODE */
#define DSI_ESCMODE (0x14)
#define DSI_ESCMODE_STOP_STATE_CNT(x) ((x) << 21)
#define DSI_ESCMODE_TX_LPDT (1 << 6)
#define DSI_ESCMODE_CMD_LPDT (1 << 7)

/* MDRESOL */
#define DSI_MDRESOL (0x18)
#define DSI_MDRESOL_STANDBY (1 << 31)
#define DSI_MDRESOL_VERTICAL(x) ((x) << 16)
#define DSI_MDRESOL_HORIZONTAL(x) ((x) << 0)

/* MVPORCH */
#define DSI_MVPORCH (0x1c)
#define DSI_MVPORCH_CMD_ALLOW(x) ((x) << 28)
#define DSI_MVPORCH_STABLE_VFP(x) ((x) << 16)
#define DSI_MVPORCH_MAIN_VBP(x) ((x) << 0)

/* MHPORCH */
#define DSI_MHPORCH (0x20)
#define DSI_MHPORCH_MAIN_HFP(x) ((x) << 16)
#define DSI_MHPORCH_MAIN_HBP(x) ((x) & 0xffff)

/* MSYNC */
#define DSI_MSYNC (0x24)
#define DSI_MSYNC_VSYNC(x) ((x) << 22)
#define DSI_MSYNC_HSYNC(x) ((x) & 0xffff)

#define DSI_SDRESOL (0x28)

/* INTSRC */
#define DSI_INTSRC (0x2c)
#define DSI_INTSRC_PLL_STABLE (1 << 31)

#define DSI_INTMSK (0x30)

/* PKTHDR */
#define DSI_PKTHDR (0x34)
#define DSI_PKTHDR_DAT1(x) ((x) << 16)
#define DSI_PKTHDR_DAT0(x) ((x) << 8)
#define DSI_PKTHDR_DI(x) (((x) & 0x3f) << 0)

#define DSI_PAYLOAD (0x38)
#define DSI_RXFIFO (0x3c)
#define DSI_FIFOTHLD (0x40)

/* FIFOCTRL */
#define DSI_FIFOCTRL (0x44)
#define DSI_FIFOCTRL_MD_FIFO (1 << 0)
#define DSI_FIFOCTRL_SD_FIFO (1 << 1)
#define DSI_FIFOCTRL_I80_FIFO (1 << 2)
#define DSI_FIFOCTRL_SFR_FIFO (1 << 3)
#define DSI_FIFOCTRL_RX_FIFO (1 << 4)

#define DSI_MEMACCHR (0x48)

/* PLLCTRL */
#define DSI_PLLCTRL (0x4c)
#define DSI_PLLCTRL_FREQBAND(x) ((x) << 24)
#define DSI_PLLCTRL_EN (1 << 23)
#define DSI_PLLCTRL_P(x) ((x) << 13)
#define DSI_PLLCTRL_M(x) ((x) << 4)
#define DSI_PLLCTRL_S(x) ((x) << 1)
#define DSI_PLLCTRL_MASK (0x7fffff << 1)

/* PLLCTRL */
#define DSI_PLLTMR (0x50)

/* PHYACCHR */
#define DSI_PHYACCHR (0x54)
#define DSI_AFC_EN (1 << 14)
#define DSI_AFC_CTL(x) ((x) << 5)
#define DSI_AFC_CTL_MASK ~(DSI_AFC_CTL(0x7))


#define DSI_PHYACCHR1 (0x58)


#endif
