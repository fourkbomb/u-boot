/* DMC */
#define CORE_TIMERS_RATIO 0x0
#define COPY2_RATIO 0x0
#define DMCP_RATIO 0x1
#define DMCD_RATIO 0x1
#define DMC_RATIO 0x3
#define DPHY_RATIO 0x1
#define ACP_PCLK_RATIO 0x1
#define ACP_RATIO 0x3

#define CLK_DIV_DMC0_VAL	((CORE_TIMERS_RATIO << 28) \
							| (COPY2_RATIO << 24) \
							| (DMCP_RATIO << 20) \
							| (DMC_RATIO << 12) \
							| (DPHY_RATIO << 8) \
							| (ACP_PCLK_RATIO << 4) \
							| (ACP_RATIO))

#define DPM_RATIO (0x7)
#define DVSEM_RATIO (0x7)
#define C2C_ACLK_RATIO (0x1)
#define PWI_RATIO (0x7)
#define C2C_RATIO (0x1)
#define G2D_ACP_RATIO (0x3)

#define CLK_DIV_DMC1_VAL	((DPM_RATIO << 24) \
							| (DVSEM_RATIO << 16) \
							| (C2C_ACLK_RATIO << 12) \
							| (PWI_RATIO << 8) \
							| (C2C_RATIO << 4) \
							| G2D_ACP_RATIO)

/* TOP */
#define MUX_ONENAND_SEL_ACLK133 0x0
#define MUX_ACLK_133_SEL_MPLL 0x0
#define MUX_ACLK_160_SEL_MPLL 0x0
#define MUX_ACLK_100_SEL_MPLL 0x0
#define MUX_ACLK_200_SEL_MPLL 0x0
#define MUX_VPLL_SEL_FOUTVPLL 0x1
#define MUX_EPLL_SEL_FOUTEPLL 0x1

#define CLK_SRC_TOP0_VAL	((MUX_ONENAND_SEL_ACLK133 << 28) \
							| (MUX_ACLK_133_SEL_MPLL << 24) \
							| (MUX_ACLK_160_SEL_MPLL << 20) \
							| (MUX_ACLK_100_SEL_MPLL << 16) \
							| (MUX_ACLK_200_SEL_MPLL << 12) \
							| (MUX_VPLL_SEL_FOUTVPLL << 8) \
							| (MUX_EPLL_SEL_FOUTEPLL << 4))

#define MUX_MCUISP_ACLK400_SEL 0x1
#define MUX_ACLK200_SEL 0x1
#define MUX_ACLK266_GPS_SEL 0x1
#define MUX_MPLL_USER_SEL 0x1

#define CLK_SRC_TOP1_VAL	((MUX_MCUISP_ACLK400_SEL << 24) \
							| (MUX_ACLK200_SEL << 20) \
							| (MUX_ACLK266_GPS_SEL << 16) \
							| (MUX_MPLL_USER_SEL << 12))

#define ACLK400_MCUISP_RATIO 0x1
#define ACLK266_GPS_RATIO 0x2
#define ONENAND_RATIO 0x1
#define ACLK133_RATIO 0x5
#define ACLK160_RATIO 0x4
#define ACLK100_RATIO 0x7
#define ACLK200_RATIO 0x4

#define CLK_DIV_TOP_VAL ((ACLK400_MCUISP_RATIO << 24) \
						| (ACLK266_GPS_RATIO << 20) \
						| (ONENAND_RATIO << 16) \
						| (ACLK133_RATIO << 12) \
						| (ACLK160_RATIO << 8) \
						| (ACLK100_RATIO << 4) \
						| (ACLK200_RATIO))

/* LEFTBUS */
#define MUX_MPLL_USER_SEL_L 0x1

#define CLK_SRC_LEFTBUS_VAL (MUX_MPLL_USER_SEL_L << 4)

#define GPL_RATIO 0x1
#define GDL_RATIO 0x3

#define CLK_DIV_LEFTBUS_VAL ((GPL_RATIO << 4) | GDL_RATIO)

/* RIGHTBUS */
#define MUX_MPLL_USER_SEL_R 0x1
#define CLK_SRC_RIGHTBUS_VAL (MUX_MPLL_USER_SEL_R << 4)

#define GPR_RATIO 0x1
#define GDR_RATIO 0x3

#define CLK_DIV_RIGHTBUS_VAL ((GPR_RATIO << 4) | GDR_RATIO)

/* CPU */
// TODO: these values are for 800MHz ARMCLK, 800MHz APLL.

#define CORE2_RATIO 0x0
#define APLL_RATIO 0x1
#define PCLK_DBG_RATIO 0x1
#define ATB_RATIO 0x3
#define PERIPH_RATIO 0x7
#define COREM1_RATIO 0x5
#define COREM0_RATIO 0x2
#define CORE_RATIO 0x0

#define CLK_DIV_CPU0_VAL	((CORE2_RATIO << 28) \
							| (APLL_RATIO << 24) \
							| (PCLK_DBG_RATIO << 20) \
							| (ATB_RATIO << 16) \
							| (PERIPH_RATIO << 12) \
							| (COREM1_RATIO << 8) \
							| (COREM0_RATIO << 4) \
							| (CORE_RATIO))
#define CORES_RATIO 0x3
#define HPM_RATIO 0x0
#define COPY_RATIO 0x3

#define CLK_DIV_CPU1_VAL	((CORES_RATIO << 8) \
							| (HPM_RATIO << 4) \
							| (COPY_RATIO))

/* APLL control */
#define RESV0 0x1
#define LOCK_CON_IN 0x3
#define LOCK_CON_DLY 0x8
#define APLL_CON1_VAL	((RESV0 << 23) \
						| (LOCK_CON_IN << 12) \
						| (LOCK_CON_DLY << 8))

#define MPLL_CON1_VAL APLL_CON1_VAL

/* PLL dividers */
#define APLL_M 0x64
#define APLL_P 0x3
#define APLL_S 0x0

// for 200MHZ BUSCLK, 400MHZ DMCCLK
#define MPLL_M 0x64
#define MPLL_P 0x3
#define MPLL_S 0x0

#define EPLL_M 0x40
#define EPLL_P 0x2
#define EPLL_S 0x3

#define VPLL_M 0x48
#define VPLL_P 0x2
#define VPLL_S 0x3

#define set_pll(m, p, s) (1 << 31 | m << 16 | p << 8 | s)

#define APLL_CON0_VAL set_pll(APLL_M, APLL_P, APLL_S)
#define MPLL_CON0_VAL set_pll(MPLL_M, MPLL_P, MPLL_S)
#define EPLL_CON0_VAL set_pll(EPLL_M, EPLL_P, EPLL_S)
#define VPLL_CON0_VAL set_pll(VPLL_M, VPLL_P, VPLL_S)

#define EPLL_CON1_VAL 0x66010000
#define EPLL_CON2_VAL 0x00000080
#define VPLL_CON1_VAL 0x66010000
#define VPLL_CON2_VAL 0x00000080
