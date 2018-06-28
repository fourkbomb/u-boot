/*
 * (C) Copyright 2011 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef _VAL_SMDKC220_H
#define _VAL_SMDKC220_H

#include <config.h>
#include <version.h>

#include <asm/arch/cpu.h>

#define EXYNOS4412_REV_ZERO 0x00
#define EXYNOS4412_REV_MAIN 0x10
#define EXYNOS4412_REV_PRIME 0x20
#define EXYNOS4412_REV_MASK 0xf0

static inline int exynos4412_get_rev(void)
{
	return readl(EXYNOS4_PRO_ID) & EXYNOS4412_REV_MASK;
}

/* CLK_DIV_CPU0 */
#define CORE2_RATIO	0x0
#define APLL_RATIO	0x1
#define PCLK_DBG_RATIO	0x1
#define ATB_RATIO	0x3
#define PERIPH_RATIO	0x7
#define COREM1_RATIO	0x5
#define COREM0_RATIO	0x2
#define CORE_RATIO	0x0

/* CLK_DIV_CPU1 */
#define CORES_RATIO	0x3
#define HPM_RATIO	0x0
#define COPY_RATIO	0x3

#define CLK_DIV_CPU0_VAL        ((CORE2_RATIO << 28)    \
                                | (APLL_RATIO << 24)    \
                                | (PCLK_DBG_RATIO << 20)\
                                | (ATB_RATIO << 16)     \
                                | (PERIPH_RATIO <<12)   \
				| (COREM1_RATIO << 8)   \
                                | (COREM0_RATIO << 4)   \
                                | (CORE_RATIO))

#define CLK_DIV_CPU1_VAL	((CORES_RATIO << 8) \
                                |  (HPM_RATIO << 4) \
                                | (COPY_RATIO))
/* APLL_CON1 / MPLL_CON1 */
#define RESV1	(1 << 24)
#define RESV0	(1 << 23)
#define BYPASS	(1 << 22)
#define DCC_ENB	(1 << 21) /* Active low */
#define AFC_ENB	(1 << 20) /* Active low */
#define FEED_EN	(1 << 16)
#define LOCK_CON_OUT(x)	((x) << 14)
#define LOCK_CON_IN(x)	((x) << 12)
#define LOCK_CON_DLY(x)	((x) << 8)
#define AFC(x)		((x) << 0)

/* MPLL */
/* 800MHz = (0x64) * 24000000 / (3 * (1 << 0)) */
#define MPLL_MDIV	0x64
#define MPLL_PDIV	0x3
#define MPLL_SDIV	0x0
#define MPLL_CON1_VAL (RESV0 | LOCK_CON_IN(3) | LOCK_CON_DLY(8))

/* ARM_CLOCK/APLL */
/* 800MHz = (0x64) * 24000000 / (3 * (1 << 0)) */
#define APLL_MDIV	0x64
#define APLL_PDIV	0x3
#define APLL_SDIV	0x0
#define APLL_CON1_VAL (RESV0 | LOCK_CON_IN(3) | LOCK_CON_DLY(8))

/* EPLL_CON1 / VPLL_CON1 */
#define SELF_PF(x)	((x) << 29)
#define MRR(x)		((x) << 24)
#define MFR(x)		((x) << 16)
#define K(x)		((x) << 0)

/* EPLL_CON2 / VPLL_CON2 */
#define EXTAFC(x)	((x) << 8)
#define DCC_ENB_EV	(1 << 7) /* Active low */
#define AFC_ENB_EV	(1 << 6) /* Active low */
#define SSCG_EN		(1 << 5)
#define BYPASS_EV	(1 << 4)
#define FVCO_EN		(1 << 3)
#define FSEL		(1 << 2)
#define ICP_BOOST(x)	((x) << 0)

/* EPLL */
/* Fout = (M + K/65536) * Fin / (P * (1 << S)) */
/* 96MHz = (0x40) * 24000000 / (2 * (1 << 3)) */
#define EPLL_MDIV	0x40
#define EPLL_PDIV	0x2
#define EPLL_SDIV	0x3

#define EPLL_CON1_VAL	SELF_PF(3) | MRR(6) | MFR(1)
#define EPLL_CON2_VAL	DCC_ENB_EV

/* VPLL */
#define VPLL_MDIV	0x48
#define VPLL_PDIV	0x2
#define VPLL_SDIV	0x3

#define VPLL_CON1_VAL	SELF_PF(3) | MRR(6) | MFR(1)
#define VPLL_CON2_VAL	DCC_ENB_EV

/* Set PLL */
#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)

#define APLL_CON0_VAL	set_pll(APLL_MDIV,APLL_PDIV,APLL_SDIV)
#define MPLL_CON0_VAL	set_pll(MPLL_MDIV,MPLL_PDIV,MPLL_SDIV)
#define EPLL_CON0_VAL	set_pll(EPLL_MDIV,EPLL_PDIV,EPLL_SDIV)
#define VPLL_CON0_VAL	set_pll(VPLL_MDIV,VPLL_PDIV,VPLL_SDIV)


/* CLK_SRC_CPU */
#define MUX_MPLL_USER_SEL_C(x)	((x) << 24) /* 0: FINPLL, 1: FOUTMPLL */
#define MUX_HPM_SEL(x)		((x) << 20) /* 0: MOUTAPLL, 1: SCLKMPLL */
#define MUX_CORE_SEL(x)		((x) << 16) /* 0: MOUTAPLL, 1: SCLKMPLL */
#define MUX_APLL_SEL(x)		((x) << 0)  /* 0: FINPLL, 1: MOUTAPLLFOUT */

/* All clocks from XusbXTI */
#define CLK_SRC_CPU_INIT	(MUX_MPLL_USER_SEL_C(0) | MUX_HPM_SEL(0) \
				| MUX_CORE_SEL(0) | MUX_APLL_SEL(0))

#define CLK_SRC_CPU_PLLS	(MUX_MPLL_USER_SEL_C(1) | MUX_APLL_SEL(1))

/* CLK_SRC_DMC */
#define MUX_PWI_SEL(x)	        ((x) << 16)	/* 0: XXTI, 1: XusbXTI, ... */
#define MUX_MPLL_SEL(x)		((x) << 12)	/* 0: FINPLL, 1: MOUTMPLLFOUT */
#define MUX_DPHY_SEL(x)		((x) << 8)	/* 0: SCLKMPLL, 1: SCLKAPLL */
#define MUX_DMC_BUS_SEL		((x) << 4)	/* 0: SCLKMPLL, 1: SCLKAPLL */

#define CLK_SRC_DMC_PLLS	(MUX_PWI_SEL(1) | MUX_MPLL_SEL(1))

/* CLK_DIV_DMC0 */
#define CORE_TIMERS_RATIO	0x0
#define COPY2_RATIO		0x0
#define DMCP_RATIO		0x1
#define DMCD_RATIO		0x1
#define DMC_RATIO		0x7
#define DPHY_RATIO		0x1
#define ACP_PCLK_RATIO		0x1
#define ACP_RATIO		0x7

#define CLK_DIV_DMC0_VAL	((CORE_TIMERS_RATIO << 28) \
				| (COPY2_RATIO << 24) \
				| (DMCP_RATIO << 20)	\
				| (DMCD_RATIO << 16)	\
				| (DMC_RATIO << 12)	\
				| (DPHY_RATIO << 8)	\
				| (ACP_PCLK_RATIO << 4)	\
				| (ACP_RATIO))

/* CLK_DIV_DMC1 */
#define DPM_RATIO		0x7
#define DVSEM_RATIO		0x7
#define C2C_ACLK_RATIO		0x1
#define PWI_RATIO		0x7
#define C2C_RATIO		0x7
#define G2D_ACP_RATIO		0x3

#define CLK_DIV_DMC1_VAL	((DPM_RATIO << 24) \
				| (DVSEM_RATIO << 16) \
				| (C2C_ACLK_RATIO << 12) \
				| (PWI_RATIO << 8) \
				| (C2C_RATIO << 4) \
				| (G2D_ACP_RATIO))

/* CLK_SRC_TOP0	*/
#define MUX_ONENAND_SEL(x)	((x) << 28) /* 0 = DOUT133, 1 = DOUT166 */
#define MUX_ACLK_133_SEL(x)	((x) << 24) /* 0 = SCLKMPLL, 1 = SCLKAPLL */
#define MUX_ACLK_160_SEL(x)	((x) << 20) /* ditto */
#define MUX_ACLK_100_SEL(x)	((x) << 16) /* ditto */
#define MUX_ACLK_200_SEL(x)	((x) << 12) /* ditto */
#define MUX_VPLL_SEL(x)		((x) << 8) /* 0: FINPLL, 1: FOUTVPLL */
#define MUX_EPLL_SEL(x)		((x) << 4) /* 0: FINPLL, 1: FOUTEPLL */
#define CLK_SRC_TOP0_INIT	(MUX_ONENAND_SEL(0)	\
				| MUX_ACLK_133_SEL(0)	\
				| MUX_ACLK_160_SEL(0)	\
				| MUX_ACLK_100_SEL(0)	\
				| MUX_ACLK_200_SEL(0)	\
				| MUX_VPLL_SEL(0)	\
				| MUX_EPLL_SEL(0))

#define CLK_SRC_TOP0_PLLS	(MUX_VPLL_SEL(1) | MUX_EPLL_SEL(1))

/* CLK_SRC_TOP1	*/
#define MUX_ACLK_400_MCUISP_SUB_SEL(x)	((x) << 24) /* 0: FINPLL, 1: DIVOUT_ACLK_400_MCUISP */
#define MUX_ACLK_200_SUB_SEL(x)		((x) << 20) /* 0: FINPLL, 1: DIVOUT_ACLK_200 */
#define MUX_ACLK_266_GPS_SUB_SEL(x)	((x) << 16) /* 0: FINPLL, 1: DIVOUT_ACLK_266_GPS */
#define MUX_MPLL_USER_T_SEL(x)		((x) << 12) /* 0: FINPLL, 1: SCLKMPLL */
#define MUX_ACLK_400_MCUISP_SEL(x)	((x) << 8)  /* 0: SCLKMPLL_USER_T, 1: SCLKAPLL */
#define MUX_ACLK_266_GPS_SEL(x)		((x) << 4)  /* 0: SCLKMPLL_USER_T, 1: SCLKAPLL */

#define CLK_SRC_TOP1_INIT	(MUX_ACLK_400_MCUISP_SUB_SEL(0)  \
				| MUX_ACLK_200_SUB_SEL(0)  \
				| MUX_ACLK_266_GPS_SUB_SEL(0)  \
				| MUX_MPLL_USER_T_SEL(0))

#define CLK_SRC_TOP1_PLLS	(MUX_ACLK_266_GPS_SUB_SEL(1) | MUX_MPLL_USER_T_SEL(1))

/* CLK_DIV_TOP */
#define ACLK_400_MCUISP_RATIO	0x1
#define ACLK_266_GPS_RATIO	0x2
#define ONENAND_RATIO	0x1
#define ACLK_133_RATIO	0x7
#define ACLK_160_RATIO	0x4
#define ACLK_100_RATIO	0xf
#define ACLK_200_RATIO	0x4

#define CLK_DIV_TOP_VAL	((ACLK_400_MCUISP_RATIO << 24) \
			| (ACLK_266_GPS_RATIO << 20) \
			| (ONENAND_RATIO << 16) \
			| (ACLK_133_RATIO << 12) \
			| (ACLK_160_RATIO << 8)	\
			| (ACLK_100_RATIO << 4)	\
			| (ACLK_200_RATIO))


/* CLK_SRC_LEFTBUS */
#define MUX_MPLL_USER_SEL_L(x)	((x) << 4) /* 0: FINPLL, 1: FOUTMPLL */
#define MUX_GDL_SEL(x)		((x) << 0) /* 0: SCLKMPLL, 1: SCLKAPLL */
#define CLK_SRC_LEFTBUS_VAL	(MUX_MPLL_USER_SEL_L(1) | MUX_GDL_SEL(0))


/* CLK_DIV_LEFTBUS */
#define GPL_RATIO	0x1
#define GDL_RATIO	0x7
#define CLK_DIV_LEFTBUS_VAL	((GPL_RATIO << 4) \
				| (GDL_RATIO))

/* CLK_SRC_RIGHTBUS */
#define MUX_MPLL_USER_SEL_R(x)	((x) << 4) /* 0: FINPLL, 1: FOUTMPLL */
#define MUX_GDR_SEL(x)		((x) << 0) /* 0: SCLKMPLL, 1: SCLKAPLL */
#define CLK_SRC_RIGHTBUS_VAL	(MUX_MPLL_USER_SEL_R(1) | MUX_GDR_SEL(0))

/* CLK_DIV_RIGHTBUS */
#define GPR_RATIO	0x1
#define GDR_RATIO	0x7
#define CLK_DIV_RIGHTBUS_VAL	((GPR_RATIO << 4) \
				| (GDR_RATIO))

/* APLL_LOCK */
#define APLL_LOCK_VAL	(0x3E8)
/* MPLL_LOCK */
#define MPLL_LOCK_VAL	(0x2F1)
/* EPLL_LOCK */
#define EPLL_LOCK_VAL	(0x2321)
/* VPLL_LOCK */
#define VPLL_LOCK_VAL	(0x2321)


/* CLK_SRC_PERIL0 */
#define UART4_SEL(x)	((x) << 16) /* 6: SCLK_MPLL_USER_T */
#define UART3_SEL(x)	((x) << 12)
#define UART2_SEL(x)	((x) << 8)
#define UART1_SEL(x)	((x) << 4)
#define UART0_SEL(x)	((x) << 0)
#define CLK_SRC_PERIL0_VAL	(UART4_SEL(6) \
				| UART3_SEL(6) \
				| UART2_SEL(6)	\
				| UART1_SEL(6)	\
				| UART0_SEL(6))

/* CLK_DIV_PERIL0 */
#define UART5_RATIO	8
#define UART4_RATIO	8
#define UART3_RATIO	8
#define UART2_RATIO	8
#define UART1_RATIO	8
#define UART0_RATIO	8

#define CLK_DIV_PERIL0_VAL	((UART5_RATIO << 20) \
				| (UART4_RATIO << 16) \
				| (UART3_RATIO << 12)	\
				| (UART2_RATIO << 8)	\
				| (UART1_RATIO << 4)	\
				| (UART0_RATIO))

/* CLK_SRC_LCD0 TODO: check this */
#define MIPI0_SEL(x)		((x) << 12) /* 1: XusbXTI */
#define MDNIE_PWM0_SEL(x)	((x) << 8)  /* 1: XusbXTI */
#define MDNIE0_SEL(x)		((x) << 4)  /* 6: SCLK_MPLL_USER_T */
#define FIMD0_SEL(x)		((x) << 0)  /* 6: SCLK_MPLL_USER_T */

#define CLK_SRC_LCD0_VAL	(MIPI0_SEL(0x1) \
				| MDNIE_PWM0_SEL(0x1) \
				| MDNIE0_SEL(0x6) \
				| FIMD0_SEL(0x6))

/* DMC PHYCONTROL0 */
#define CTRL_FORCE	(0x7f << 24)
#define CTRL_INC	(0x10 << 16)
#define CTRL_START_POINT (0x10 << 8)
#define DQS_DELAY	(0x0 << 4)
#define CTRL_DFDQS	(0x1 << 3)
#define CTRL_HALF	(0x0 << 2)
#define CTRL_DLL_ON	(0x1 << 1)
#define CTRL_DLL_START (0x1 << 0)

/* CTRL_DLL_START will be ORd in when appropriate */
#define PHYCONTROL0_VAL (CTRL_FORCE | CTRL_INC \
			| CTRL_START_POINT | DQS_DELAY \
			| CTRL_DFDQS | CTRL_HALF | CTRL_DLL_ON)

/* DMC PHYCONTROL1 */
#define MEM_TERM_EN (0x1 << 31)
#define PHY_READ_EN (0x1 << 30)
#define CTRL_SHGATE (0x1 << 29)
#define FP_RESYNC	(0x1 << 3)
#define CTRL_REF(x)	(x << 4)
#define CTRL_SHIFTC(x) (x << 0)

#define DMC_PHYCONTROL1 (MEM_TERM_EN | PHY_READ_EN \
			| CTRL_SHGATE | CTRL_REF | CTRL_SHIFTC)

/* DMC CONCONTROL */
#define TIMEOUT_LEVEL0	(0xfff << 16)
#define RD_FETCH		(0x3 << 12)
#define DRV_TYPE		(0x3 << 6)
#define AREF_EN			(0x1 << 5)
#define PDN_DQ_DISABLE	(0x1 << 4)
#define IO_PDN_CON		(0x1 << 3)
#define CLK_RATIO		(0x1 << 1)

#define DMC_CONCONTROL	(TIMEOUT_LEVEL0 | RD_FETCH \
			| DRV_TYPE | AREF_EN \
			| PDN_DQ_DISABLE | IO_PDN_CON \
			| CLK_RATIO)

/* DMC MEMCONTROL */
#define BURSTLEN	(0x2 << 20)
#define MEM_WIDTH	(0x2 << 12)
#define MEM_2CHIPS	(0x1 << 16)
#define MEM_TYPE	(0x5 << 8) /* LPDDR2-S4 */
#define DSREF_EN	(0x1 << 5)
#define TP_EN		(0x1 << 4)
#define DPWRDN_EN	(0x1 << 1)
#define CLK_STOP_EN (0x1 << 0)

#define DMC_MEMCONTROL (BURSTLEN | MEM_WIDTH | MEM_TYPE)

#endif

