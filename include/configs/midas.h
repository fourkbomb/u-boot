#ifndef __MIDAS_CONFIG_H__
#define __MIDAS_CONFIG_H__

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


/* Boot off EMMC */
#define CONFIG_SUPPORT_EMMC_BOOT

#define CONFIG_IPADDR 192.168.168.2

#define CONFIG_EXTRA_ENV_SETTINGS \
	"fitloadaddr=0x50000000\0" /* load address of FIT image */\
	"nc=" \
		"setenv stdout nc;setenv stdin nc\0" \
	"sd=" /* load kernel from SD card */\
		"mmc rescan; mmc dev 1; mmc read ${fitloadaddr} 0x800 0xa000;" \
		"run bootimage;\0" \
	"sdupdate=" /* install updated u-boot from partitioned SD card */ \
		"mmc dev 1 && mmc read ${fitloadaddr} 0xa800 0x1000 && " \
		"mmc dev 0 1 && mmc write ${fitloadaddr} 0x0 0x1000;\0" \
	"bootsdupdate=" /* install updated u-boot from bootable SD card */ \
		"mmc dev 1 && mmc read ${fitloadaddr} 0x1 0x1000 &&" \
		"mmc dev 0 1 && mmc write ${fitloadaddr} 0x0 0x1000;\0" \
	"bootimage=" /* Boot loaded image */ \
		"run setbootargs; if test ${fit_config} = \"\"; then; bootm ${fitloadaddr}; else; bootm ${fitloadaddr}#${fit_config}${lcd_overlay}; fi\0" \
	"mmcboot=" /* Command to boot OS from eMMC */ \
		"read mmc 0 boot ${fitloadaddr} 0x0 end && run bootimage; run fastboot\0" \
	"mmcrecovery=" /* Command to boot recovery from eMMC */ \
		"read mmc 0 recovery ${fitloadaddr} 0x0 end && run bootimage; run fastboot\0" \
	"autoboot=run mmcboot\0" /* Run on normal boot */ \
	"recoveryboot=run mmcrecovery\0" /* Run on recovery keycombo/INFORM3 value */ \
	/* fastboot may have wrapped our FIT image in an Android boot.img, which means the FIT will be one page (2048 bytes by default) past ${fitloadaddr} */ \
	"fastboot_bootcmd=if fdt addr ${fitloadaddr}; then; else; setexpr fitloadaddr ${fitloadaddr} + 0x800; fdt addr ${fitloadaddr}; fi; run bootimage\0" \
	"fastboot=mw ${fitloadaddr} 0x0 0x100; fastboot -l ${fitloadaddr} 0; run autoboot\0" /* Run on fastboot keycombo/INFORM3 value */ \
	"readsb20=setexpr.l h01 *0x50001fb0 && setexpr.l h02 *0x50001fb4 && setexpr.l h03 *0x50001fb8\0" /* In a valid BL1, this will be "SB20_CONTEXT" */ \
	"checksb20=test ${h01} = 30324253 && test ${h02} = 4e4f435f && test ${h03} = 54584554\0" \
	"clearsb20=env delete h01 h02 h03\0" \
	"dommcupdate=" /* install u-boot in bootloader partition to actual partition */ \
		"read mmc 0 bootloader ${fitloadaddr} 0x0 end &&" \
		"run readsb20 && run checksb20 && mmc dev 0 1 && mmc write ${fitloadaddr} 0x0 0x1000 && run clearsb20\0" \
	"mmcupdate=if run dommcupdate; then echo Bootloader upgrade succeeded; else echo Bootloader upgrade failed.; fi\0" \
	"bootargs=console=ttySAC2,115200\0" \
	"readfitbootargs=fdt addr ${fitloadaddr}; fdt get value newargs / cmdline && setenv bootargs ${bootargs} ${newargs}\0" \
	"setbootargs=run readfitbootargs; setenv bootargs ${bootargs} androidboot.mode=${bootmode} androidboot.revision=${board_rev} androidboot.serialno=${serial#}\0" \
	"machid=eb5\0" \

#include <linux/sizes.h>

#define CONFIG_BAUDRATE		115200

#endif
