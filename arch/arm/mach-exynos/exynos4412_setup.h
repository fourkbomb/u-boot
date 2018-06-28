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


/* ARM_CLOCK_800Mhz */
#if 1//defined(CONFIG_CLK_ARM_800_APLL_800
#define APLL_MDIV	0x64
#define APLL_PDIV	0x3
#define APLL_SDIV	0x0

/* CLK_DIV_CPU0	*/
#define APLL_RATIO	0x1
#define PCLK_DBG_RATIO	0x1
#define ATB_RATIO	0x3
#define PERIPH_RATIO	0x7
#define COREM1_RATIO	0x5
#define COREM0_RATIO	0x2
#define CORE_RATIO	0x0
#define CORE2_RATIO	0x0

/* CLK_DIV_CPU1	*/
#define CORES_RATIO	0x3
#define HPM_RATIO	0x0
#define COPY_RATIO	0x3

/* ARM_CLOCK_1Ghz */
#elif defined(CONFIG_CLK_ARM_1000_APLL_1000)
#define APLL_MDIV	0x7D
#define APLL_PDIV	0x3
#define APLL_SDIV	0x0

/* CLK_DIV_CPU0	*/
#define APLL_RATIO	0x1
#define CORE_RATIO	0x0
#define CORE2_RATIO	0x0
#define COREM0_RATIO	0x2
#define COREM1_RATIO	0x5
#define PERIPH_RATIO	0x7
#define ATB_RATIO	0x4
#define PCLK_DBG_RATIO	0x1

/* CLK_DIV_CPU1	*/
#define CORES_RATIO	0x3
#define HPM_RATIO	0x0
#define COPY_RATIO	0x4

/* ARM_CLOCK_1.2Ghz */
#elif defined(CONFIG_CLK_ARM_1200_APLL_1100)
#define APLL_MDIV	0x113
#define APLL_PDIV	0x6
#define APLL_SDIV	0x0

#define APLL_RATIO	0x2
#define CORE_RATIO	0x0
#define CORE2_RATIO	0x0
#define COREM0_RATIO	0x3
#define COREM1_RATIO	0x6
#define PERIPH_RATIO	0x7
#define ATB_RATIO	0x4
#define PCLK_DBG_RATIO	0x1

/* CLK_DIV_CPU1	*/
#define CORES_RATIO	0x4
#define HPM_RATIO	0x0
#define COPY_RATIO	0x4

/* ARM_CLOCK_1.2Ghz */
#elif defined(CONFIG_CLK_ARM_1200_APLL_1200)
#define APLL_MDIV	0x96
#define APLL_PDIV	0x3
#define APLL_SDIV	0x0

#define APLL_RATIO	0x2
#define CORE_RATIO	0x0
#define CORE2_RATIO	0x0
#define COREM0_RATIO	0x3
#define COREM1_RATIO	0x7
#define PERIPH_RATIO	0x7
#define ATB_RATIO	0x5
#define PCLK_DBG_RATIO	0x1

/* CLK_DIV_CPU1	*/
#define CORES_RATIO	0x4
#define HPM_RATIO	0x0
#define COPY_RATIO	0x5

/* ARM_CLOCK_1.2Ghz */
#elif defined(CONFIG_CLK_ARM_1200_APLL_1300)
#define APLL_MDIV	0x145
#define APLL_PDIV	0x6
#define APLL_SDIV	0x0

#define APLL_RATIO	0x2
#define CORE_RATIO	0x0
#define CORE2_RATIO	0x0
#define COREM0_RATIO	0x3
#define COREM1_RATIO	0x7
#define PERIPH_RATIO	0x7
#define ATB_RATIO	0x5
#define PCLK_DBG_RATIO	0x1

/* CLK_DIV_CPU1	*/
#define CORES_RATIO	0x5
#define HPM_RATIO	0x0
#define COPY_RATIO	0x5

/* ARM_CLOCK_1.2Ghz */
#elif defined(CONFIG_CLK_ARM_1200_APLL_1400)
#define APLL_MDIV	0xAF
#define APLL_PDIV	0x3
#define APLL_SDIV	0x0

#define APLL_RATIO	0x2
#define CORE_RATIO	0x0
#define CORE2_RATIO	0x0
#define COREM0_RATIO	0x3
#define COREM1_RATIO	0x7
#define PERIPH_RATIO	0x7
#define ATB_RATIO	0x6
#define PCLK_DBG_RATIO	0x1

/* CLK_DIV_CPU1	*/
#define CORES_RATIO	0x5
#define HPM_RATIO	0x0
#define COPY_RATIO	0x6

/* ARM_CLOCK_1.5Ghz */
#elif defined(CONFIG_CLK_ARM_1500_APLL_1500)
#define APLL_MDIV	0xFA
#define APLL_PDIV	0x4
#define APLL_SDIV	0x0

#define APLL_RATIO	0x2
#define CORE_RATIO	0x0
#define CORE2_RATIO	0x0
#define COREM0_RATIO	0x4
#define COREM1_RATIO	0x7
#define PERIPH_RATIO	0x7
#define ATB_RATIO	0x6
#define PCLK_DBG_RATIO	0x1

/* CLK_DIV_CPU1	*/
#define CORES_RATIO	0x5
#define HPM_RATIO	0x0
#define COPY_RATIO	0x6

#endif

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

#if 0 //defined(CONFIG_CLK_BUS_DMC_165_330)
#define MPLL_MDIV	0x116
#define MPLL_PDIV	0x5
#define MPLL_SDIV	0x1
#else //defined(CONFIG_CLK_BUS_DMC_200_400)
#define MPLL_MDIV	0x64
#define MPLL_PDIV	0x3
#define MPLL_SDIV	0x0
#endif
#if 0 //defined(CONFIG_CLK_BUS_DMC_100_200)
#define MPLL_MDIV	0x64
#define MPLL_PDIV	0x3
#define MPLL_SDIV	0x0
#endif

/* APLL_CON1	*/
#define APLL_CON1_VAL	(0x00803800)

/* MPLL_CON1	*/
#define MPLL_CON1_VAL (0x00803800)

#define EPLL_MDIV	0x40
#define EPLL_PDIV	0x2
#define EPLL_SDIV	0x3

#define EPLL_CON1_VAL	0x66010000
#define EPLL_CON2_VAL	0x00000080

#define VPLL_MDIV	0x48
#define VPLL_PDIV	0x2
#define VPLL_SDIV	0x3

#define VPLL_CON1_VAL	0x66010000
#define VPLL_CON2_VAL	0x00000080

/* Set PLL */
#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)

#define APLL_CON0_VAL	set_pll(APLL_MDIV,APLL_PDIV,APLL_SDIV)
#define MPLL_CON0_VAL	set_pll(MPLL_MDIV,MPLL_PDIV,MPLL_SDIV)
#define EPLL_CON0_VAL	set_pll(EPLL_MDIV,EPLL_PDIV,EPLL_SDIV)
#define VPLL_CON0_VAL	set_pll(VPLL_MDIV,VPLL_PDIV,VPLL_SDIV)


/* CLK_SRC_CPU	*/
/* 0 = MOUTAPLL,  1 = SCLKMPLL	*/
#define MUX_HPM_SEL_MOUTAPLL	0
#define MUX_HPM_SEL_SCLKMPLL	1
#define MUX_CORE_SEL_MOUTAPLL	0
#define MUX_CORE_SEL_SCLKMPLL	1

/* 0 = FILPLL, 1 = MOUT */
#define MUX_MPLL_SEL_FILPLL	0
#define MUX_MPLL_SEL_MOUTMPLLFOUT	1

#define MUX_APLL_SEL_FILPLL	0
#define MUX_APLL_SEL_MOUTMPLLFOUT	1

#define CLK_SRC_CPU_VAL_FINPLL	        ((MUX_HPM_SEL_MOUTAPLL << 20)    \
                                | (MUX_CORE_SEL_MOUTAPLL <<16)   \
                                | (MUX_MPLL_SEL_FILPLL << 8)   \
                                | (MUX_APLL_SEL_FILPLL <<0))

#define CLK_SRC_CPU_VAL_MOUTMPLLFOUT		((MUX_HPM_SEL_MOUTAPLL << 20)    \
                                | (MUX_CORE_SEL_MOUTAPLL <<16)   \
                                | (MUX_MPLL_SEL_MOUTMPLLFOUT << 8)   \
                                | (MUX_APLL_SEL_MOUTMPLLFOUT <<0))



/* CLK_SRC_DMC	*/
#define MUX_PWI_SEL	        0x0
#define MUX_CORE_TIMERS_SEL	0x0
#define MUX_DPHY_SEL		0x0
#define MUX_DMC_BUS_SEL		0x0
#define CLK_SRC_DMC_VAL         ((MUX_PWI_SEL << 16)            \
                                | (MUX_CORE_TIMERS_SEL << 12)   \
                                | (MUX_DPHY_SEL << 8)           \
                                | (MUX_DMC_BUS_SEL << 4))



/* CLK_DIV_DMC0	*/
#define CORE_TIMERS_RATIO	0x0
#define COPY2_RATIO		0x0
#define DMCP_RATIO		0x1
#define DMCD_RATIO		0x1
#if 0 //defined(CONFIG_CLK_BUS_DMC_100_200)
#define DMC_RATIO		0x3
#else
#define DMC_RATIO		0x7
#endif
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
#define C2C_ACLK_RATIO	0x1
#define PWI_RATIO		0x7
#define C2C_RATIO		0x7
#define G2D_ACP_RATIO	0x3

#define CLK_DIV_DMC1_VAL	((DPM_RATIO << 24) \
							| (DVSEM_RATIO << 16) \
							| (C2C_ACLK_RATIO << 12) \
							| (PWI_RATIO << 8) \
							| (C2C_RATIO << 4) \
							| (G2D_ACP_RATIO))

/* CLK_SRC_TOP0	*/
#define MUX_ONENAND_SEL 0x0 /* 0 = DOUT133, 1 = DOUT166		*/
#define MUX_ACLK_133_SEL	0x0	/* 0 = SCLKMPLL, 1 = SCLKAPLL	*/
#define MUX_ACLK_160_SEL	0x0 /* MPLL */
#define MUX_ACLK_100_SEL	0x0 /* MPLL */
#define MUX_ACLK_200_SEL	0x0 /* MPLL */
#define MUX_VPLL_SEL	0x1 /* FOUTVPLL */
#define MUX_EPLL_SEL	0x1 /* FOUTEPLL */
#define CLK_SRC_TOP0_VAL	((MUX_ONENAND_SEL << 28)	\
							| (MUX_ACLK_133_SEL << 24)	\
							| (MUX_ACLK_160_SEL << 20)	\
							| (MUX_ACLK_100_SEL << 16)	\
							| (MUX_ACLK_200_SEL << 12)	\
							| (MUX_VPLL_SEL << 8)	\
							| (MUX_EPLL_SEL << 4))

/* CLK_SRC_TOP1	*/
#define VPLLSRC_SEL	0x0	/* 0 = FINPLL, 1 = SCLKHDMI27M	*/
#define MUX_ACLK_400_MCUISP_SUB_SEL 0x0 /*FINPLL*/
#define MUX_ACLK_200_SUB_SEL 0x0 /* FINPLL */
#define MUX_ACLK_266_GPS_SUB_SEL 0x1 /*DIVOUT_ACLK_266_GPS*/
#define MUX_MPLL_USER_T_SEL 0x1 /* SCLKMPLL */

#define CLK_SRC_TOP1_VAL	((MUX_ACLK_400_MCUISP_SUB_SEL << 24) \
							| (MUX_ACLK_200_SUB_SEL << 20) \
							| (MUX_ACLK_266_GPS_SUB_SEL << 16) \
							| (MUX_MPLL_USER_T_SEL << 12))

/* CLK_DIV_TOP	*/
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


/* CLK_SRC_LEFTBUS	*/
#define CLK_SRC_LEFTBUS_VAL	(0x10) /* MPLL, GDL from MPLL */


/* CLK_DIV_LEFTBUS	*/
#define GPL_RATIO	0x1
#define GDL_RATIO	0x7
#define CLK_DIV_LEFTBUS_VAL	((GPL_RATIO << 4) \
								| (GDL_RATIO))

/* CLK_SRC_RIGHTBUS	*/
#define CLK_SRC_RIGHTBUS_VAL	(0x10)


/* CLK_DIV_RIGHTBUS	*/
#define GPR_RATIO	0x1
#define GDR_RATIO	0x7
#define CLK_DIV_RIGHTBUS_VAL	((GPR_RATIO << 4) \
								| (GDR_RATIO))

/* APLL_LOCK	*/
#define APLL_LOCK_VAL	(0x3E8)
/* MPLL_LOCK	*/
#define MPLL_LOCK_VAL	(0x2F1)
/* EPLL_LOCK	*/
#define EPLL_LOCK_VAL	(0x2321)
/* VPLL_LOCK	*/
#define VPLL_LOCK_VAL	(0x2321)


/* CLK_SRC_PERIL0	*/
#define PWM_SEL		0
#define UART5_SEL	6
#define UART4_SEL	6
#define UART3_SEL	6
#define UART2_SEL	6
#define UART1_SEL	6
#define UART0_SEL	6
#define CLK_SRC_PERIL0_VAL	((PWM_SEL << 24)\
								| (UART5_SEL << 20)  \
								| (UART4_SEL << 16) \
								| (UART3_SEL << 12) \
								| (UART2_SEL<< 8)	\
								| (UART1_SEL << 4)	\
								| (UART0_SEL))

/* CLK_DIV_PERIL0	*/
#if 0 //defined(CONFIG_CLK_BUS_DMC_165_330)
#define UART5_RATIO	7
#define UART4_RATIO	7
#define UART3_RATIO	7
#define UART2_RATIO	7
#define UART1_RATIO	7
#define UART0_RATIO	7
#else // defined(CONFIG_CLK_BUS_DMC_200_400)
#define UART5_RATIO	8
#define UART4_RATIO	8
#define UART3_RATIO	8
#define UART2_RATIO	8
#define UART1_RATIO	8
#define UART0_RATIO	8
#endif

#define CLK_DIV_PERIL0_VAL	((UART5_RATIO << 20) \
								| (UART4_RATIO << 16) \
								| (UART3_RATIO << 12)	\
								| (UART2_RATIO << 8)	\
								| (UART1_RATIO << 4)	\
								| (UART0_RATIO))


#define MPLL_DEC	(MPLL_MDIV * MPLL_MDIV / (MPLL_PDIV * 2^(MPLL_SDIV-1)))


#define SCLK_UART	MPLL_DEC / (UART1_RATIO+1)

#if 0 //defined(CONFIG_CLK_BUS_DMC_165_330)
#define UART_UBRDIV_VAL	0x2B/* (SCLK_UART/(115200*16) -1) */
#define UART_UDIVSLOT_VAL	0xC		/*((((SCLK_UART*10/(115200*16) -10))%10)*16/10)*/
#else // defined(CONFIG_CLK_BUS_DMC_200_400)
#define UART_UBRDIV_VAL	0x2F     /* (SCLK_UART/(115200*16) -1) */
#define UART_UDIVSLOT_VAL 0x3		/*((((SCLK_UART*10/(115200*16) -10))%10)*16/10)*/
#endif

/* CLK_SRC_LCD0 */
#define MIPI0_SEL 0x1
#define MDNIE_PWM0_SEL 0x1
#define MDNIE0_SEL 0x6
#define FIMD0_SEL 0x6

#define CLK_SRC_LCD0_VAL	((MIPI0_SEL << 12) \
							| (MDNIE_PWM0_SEL << 8) \
							| (MDNIE0_SEL << 4) \
							| (FIMD0_SEL))

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

