#ifndef __I9300_CONFIG_H__
#define __I9300_CONFIG_H__

#define CONFIG_EXYNOS4

#include <configs/exynos4-common.h>


/* Cache disabled */
#define CONFIG_SYS_L2CACHE_OFF

/* 1GB of DRAM */
#define CONFIG_NR_DRAM_BANKS 4
#define CONFIG_SYS_SDRAM_BASE 0x40000000
#define SDRAM_BANK_SIZE (256 << 20) /*0x40000000 * 1GB */ /* 256MB */
#define PHYS_SDRAM_1 CONFIG_SYS_SDRAM_BASE

/* Full U-Boot memory setup */
#define CONFIG_SYS_LOAD_ADDR (CONFIG_SYS_SDRAM_BASE + 0x3E00000)

/* Enable SPL */
#define CONFIG_EXYNOS_SPL
#define CONFIG_SPL_TEXT_BASE 0x02023400
#define CONFIG_SPL_STACK 0x02050000
#define CONFIG_IRAM_TOP 0x02050000
#define CONFIG_SYS_INIT_SP_ADDR 0x02050000
#define CONFIG_SPL_MAX_FOOTPRINT ((14 << 10) - 4)

/*
 * SD boot
 * 512b reserved
 * 8K BL1
 * 14K SPL
 * 512K u-boot (?)
 * 16K env
 */
#define CONFIG_RES_BLOCK_SIZE (512)
#define CONFIG_BL1_SIZE (8 << 10) /* 8K for BL1 */
#define CONFIG_SPL_SIZE (16 << 10) /* 14K for SPL (BL2). 4 byte checksum. Padded to 16K */
#define CONFIG_BL2_SIZE (1024 << 10) /* 1024K u-boot */
#define CONFIG_SEC_FW_SIZE (160 << 10) /* 160K TZSW */
#define CONFIG_ENV_SIZE (16 << 10) /* 16K for env */

#define CONFIG_BL2_OFFSET (CONFIG_RES_BLOCK_SIZE + CONFIG_BL1_SIZE + CONFIG_SPL_SIZE)

#define BL2_START_OFFSET (CONFIG_BL2_OFFSET/512)
#define BL2_SIZE_BLOC_COUNT (CONFIG_BL2_SIZE/512)


/* Boot off EMMC */
/*#define CONFIG_SUPPORT_EMMC_BOOT*/

#include <linux/sizes.h>

#define CONFIG_SERIAL2
#define CONFIG_BAUDRATE		115200

#endif
