#ifndef __I9300_CONFIG_H__
#define __I9300_CONFIG_H__

#define CONFIG_EXYNOS4

#include <configs/exynos4-common.h>


/* Cache disabled */
#define CONFIG_SYS_L2CACHE_OFF
#ifndef CONFIG_SYS_L2CACHE_OFF
#define CONFIG_SYS_L2_PL310
#define CONFIG_SYS_PL310_BASE	0x10502000
#endif

#define CONFIG_MISC_COMMON
#define CONFIG_MISC_INIT_R
#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
#define CONFIG_REVISION_TAG

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
 * SD boot: note that this is not used when booting off the eMMC.
 * 512b reserved
 * 8K BL1
 * 14K SPL, padded to 16K
 * 1024K u-boot (?)
 */
#define CONFIG_RES_BLOCK_SIZE (512)
#define CONFIG_BL1_SIZE (8 << 10) /* 8K for BL1 */
#define CONFIG_SPL_SIZE (16 << 10) /* 14K for SPL (BL2). 4 byte checksum. Padded to 16K */
#define CONFIG_BL2_SIZE (1024 << 10) /* 1024K u-boot */

#define CONFIG_BL2_OFFSET (CONFIG_RES_BLOCK_SIZE + CONFIG_BL1_SIZE + CONFIG_SPL_SIZE)

#define BL2_START_OFFSET (CONFIG_BL2_OFFSET/512)
#define BL2_SIZE_BLOC_COUNT (CONFIG_BL2_SIZE/512)

/* env on eMMC, in the "bootinfo" partition */
#define CONFIG_SYS_MMC_ENV_DEV 0
#define CONFIG_SYS_MMC_ENV_PART 0
#define CONFIG_ENV_SIZE (64 << 10) /* 64K for env */
#define CONFIG_ENV_OFFSET (32 << 10) /* env at LBA 0x40 */


/* Boot off EMMC */
#define CONFIG_SUPPORT_EMMC_BOOT

#define CONFIG_IPADDR 192.168.4.1

#define CONFIG_EXTRA_ENV_SETTINGS \
	"loadk=" \
		"mmc dev 0 0; mmc read 0x50000000 0x14000 0x4000;" \
		"load mmc 0:a 0x60000000 /exynos4412-midas.dtb\0" \
	"nc=" \
		"setenv stdout nc;setenv stdin nc\0" \
	"sd=" /* load kernel from SD card */\
		"mmc rescan; mmc dev 1; mmc read 0x50000000 0x800 0xa000;" \
		"bootm 0x50000000;\0" \
	"update=" /* install updated u-boot from partitioned SD card */ \
		"mmc dev 1; mmc read 0x50000000 0xa800 0x1000;" \
		"mmc dev 0 1; mmc write 0x50000000 0x0 0x1000;\0" \
	"sdupdate=" /* install updated u-boot from bootable SD card */ \
		"mmc dev 1; mmc read 0x50000000 0x1 0x1000;" \
		"mmc dev 0 1; mmc write 0x50000000 0x0 0x1000;\0" \
	"mmcbootpart=3\0" \
	"mmcrecoverypart=5\0" \
	"mmcbootinfopart=0\0" \
	"mmcboot=" \
		"read mmc 0:${mmcbootpart} 0x50000000 0x0 0xbfff; bootm 0x50000000\0" \
	"mmcrecovery=" \
		"read mmc 0:${mmcrecoverypart} 0x50000000 0x0 0xbfff; bootm 0x50000000\0"

#include <linux/sizes.h>

#define CONFIG_SERIAL2
#define CONFIG_BAUDRATE		115200

#endif
