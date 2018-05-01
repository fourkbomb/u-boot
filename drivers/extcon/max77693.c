#include <common.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <extcon.h>
#include <power/pmic.h>
#include <power/max77693_pmic.h>
#include <power/max77693_muic.h>

/* ADC accessory types */
enum max77693_muic_acc_type {
	MAX77693_MUIC_ADC_GROUND = 0x0,
	MAX77693_MUIC_ADC_SEND_END_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S1_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S2_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S3_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S4_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S5_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S6_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S7_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S8_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S9_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S10_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S11_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S12_BUTTON,
	MAX77693_MUIC_ADC_RESERVED_ACC_1,
	MAX77693_MUIC_ADC_RESERVED_ACC_2,
	MAX77693_MUIC_ADC_RESERVED_ACC_3,
	MAX77693_MUIC_ADC_RESERVED_ACC_4,
	MAX77693_MUIC_ADC_RESERVED_ACC_5,
	MAX77693_MUIC_ADC_CEA936_AUDIO,
	MAX77693_MUIC_ADC_PHONE_POWERED_DEV,
	MAX77693_MUIC_ADC_TTY_CONVERTER,
	MAX77693_MUIC_ADC_UART_CABLE,
	MAX77693_MUIC_ADC_CEA936A_TYPE1_CHG,
	MAX77693_MUIC_ADC_FACTORY_MODE_USB_OFF,
	MAX77693_MUIC_ADC_FACTORY_MODE_USB_ON,
	MAX77693_MUIC_ADC_AV_CABLE_NOLOAD,
	MAX77693_MUIC_ADC_CEA936A_TYPE2_CHG,
	MAX77693_MUIC_ADC_FACTORY_MODE_UART_OFF,
	MAX77693_MUIC_ADC_FACTORY_MODE_UART_ON,
	MAX77693_MUIC_ADC_AUDIO_MODE_REMOTE,
	MAX77693_MUIC_ADC_OPEN,

	/*
	 * The below accessories have same ADC value so ADCLow and
	 * ADC1K bit is used to separate specific accessory.
	 */
						/* ADC|VBVolot|ADCLow|ADC1K| */
	MAX77693_MUIC_GND_USB_HOST = 0x100,	/* 0x0|      0|     0|    0| */
	MAX77693_MUIC_GND_USB_HOST_VB = 0x104,	/* 0x0|      1|     0|    0| */
	MAX77693_MUIC_GND_AV_CABLE_LOAD = 0x102,/* 0x0|      0|     1|    0| */
	MAX77693_MUIC_GND_MHL = 0x103,		/* 0x0|      0|     1|    1| */
	MAX77693_MUIC_GND_MHL_VB = 0x107,	/* 0x0|      1|     1|    1| */
};


static int max77693_get_cable_id(struct udevice *dev)
{
	uint8_t status[2];
	int ret;

	ret = pmic_read(dev->parent, MAX77693_MUIC_STATUS1, status, 2);
	if (ret)
		return ret;

	pr_info("%s: status=%#x,%#x\n", __func__, status[0], status[1]);

	/* For now, it's either a charging cable, UART, or nothing. */
	if ((status[1] & MAX77693_MUIC_CHG_MASK) == MAX77693_MUIC_CHG_NO) {
		int adc1 = status[0] & MAX77693_MUIC_ADC_MASK;
		switch (adc1) {
			case MAX77693_MUIC_ADC_FACTORY_MODE_UART_ON:
			case MAX77693_MUIC_ADC_FACTORY_MODE_UART_OFF:
				return EXTCON_JIG;
			default:
				return EXTCON_NONE;
		}
	}

	return EXTCON_CHG_USB_SLOW;
}

static int max77693_get_max_charge_current(struct udevice *dev) {
	int type = max77693_get_cable_id(dev);
	if (type < 0)
		return type;

	if (type == EXTCON_CHG_USB_SLOW)
		return 460 * 1000;

	return 0;
}


static const struct dm_extcon_ops max77693_extcon_ops = {
	.get_cable_id = max77693_get_cable_id,
	.get_max_charge_current = max77693_get_max_charge_current,
};

U_BOOT_DRIVER(extcon_max77693) = {
	.name = MAX77693_MUIC_DRIVER,
	.id = UCLASS_EXTCON,
	.ops = &max77693_extcon_ops,
};

