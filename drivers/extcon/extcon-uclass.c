#include <common.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <extcon.h>

DECLARE_GLOBAL_DATA_PTR;

static const struct __extcon_info {
	unsigned int type;
	unsigned int id;
	const char *name;
} extcon_info[] = {
	[EXTCON_NONE] = {
		.type = EXTCON_TYPE_MISC,
		.id = EXTCON_NONE,
		.name = "NONE",
	},

	/* USB external connector */
	[EXTCON_USB] = {
		.type = EXTCON_TYPE_USB,
		.id = EXTCON_USB,
		.name = "USB",
	},
	[EXTCON_USB_HOST] = {
		.type = EXTCON_TYPE_USB,
		.id = EXTCON_USB_HOST,
		.name = "USB-HOST",
	},

	/* Charging external connector */
	[EXTCON_CHG_USB_SDP] = {
		.type = EXTCON_TYPE_CHG | EXTCON_TYPE_USB,
		.id = EXTCON_CHG_USB_SDP,
		.name = "SDP",
	},
	[EXTCON_CHG_USB_DCP] = {
		.type = EXTCON_TYPE_CHG | EXTCON_TYPE_USB,
		.id = EXTCON_CHG_USB_DCP,
		.name = "DCP",
	},
	[EXTCON_CHG_USB_CDP] = {
		.type = EXTCON_TYPE_CHG | EXTCON_TYPE_USB,
		.id = EXTCON_CHG_USB_CDP,
		.name = "CDP",
	},
	[EXTCON_CHG_USB_ACA] = {
		.type = EXTCON_TYPE_CHG | EXTCON_TYPE_USB,
		.id = EXTCON_CHG_USB_ACA,
		.name = "ACA",
	},
	[EXTCON_CHG_USB_FAST] = {
		.type = EXTCON_TYPE_CHG | EXTCON_TYPE_USB,
		.id = EXTCON_CHG_USB_FAST,
		.name = "FAST-CHARGER",
	},
	[EXTCON_CHG_USB_SLOW] = {
		.type = EXTCON_TYPE_CHG | EXTCON_TYPE_USB,
		.id = EXTCON_CHG_USB_SLOW,
		.name = "SLOW-CHARGER",
	},
	[EXTCON_CHG_WPT] = {
		.type = EXTCON_TYPE_CHG,
		.id = EXTCON_CHG_WPT,
		.name = "WPT",
	},
	[EXTCON_CHG_USB_PD] = {
		.type = EXTCON_TYPE_CHG | EXTCON_TYPE_USB,
		.id = EXTCON_CHG_USB_PD,
		.name = "PD",
	},

	/* Jack external connector */
	[EXTCON_JACK_MICROPHONE] = {
		.type = EXTCON_TYPE_JACK,
		.id = EXTCON_JACK_MICROPHONE,
		.name = "MICROPHONE",
	},
	[EXTCON_JACK_HEADPHONE] = {
		.type = EXTCON_TYPE_JACK,
		.id = EXTCON_JACK_HEADPHONE,
		.name = "HEADPHONE",
	},
	[EXTCON_JACK_LINE_IN] = {
		.type = EXTCON_TYPE_JACK,
		.id = EXTCON_JACK_LINE_IN,
		.name = "LINE-IN",
	},
	[EXTCON_JACK_LINE_OUT] = {
		.type = EXTCON_TYPE_JACK,
		.id = EXTCON_JACK_LINE_OUT,
		.name = "LINE-OUT",
	},
	[EXTCON_JACK_VIDEO_IN] = {
		.type = EXTCON_TYPE_JACK,
		.id = EXTCON_JACK_VIDEO_IN,
		.name = "VIDEO-IN",
	},
	[EXTCON_JACK_VIDEO_OUT] = {
		.type = EXTCON_TYPE_JACK,
		.id = EXTCON_JACK_VIDEO_OUT,
		.name = "VIDEO-OUT",
	},
	[EXTCON_JACK_SPDIF_IN] = {
		.type = EXTCON_TYPE_JACK,
		.id = EXTCON_JACK_SPDIF_IN,
		.name = "SPDIF-IN",
	},
	[EXTCON_JACK_SPDIF_OUT] = {
		.type = EXTCON_TYPE_JACK,
		.id = EXTCON_JACK_SPDIF_OUT,
		.name = "SPDIF-OUT",
	},

	/* Display external connector */
	[EXTCON_DISP_HDMI] = {
		.type = EXTCON_TYPE_DISP,
		.id = EXTCON_DISP_HDMI,
		.name = "HDMI",
	},
	[EXTCON_DISP_MHL] = {
		.type = EXTCON_TYPE_DISP,
		.id = EXTCON_DISP_MHL,
		.name = "MHL",
	},
	[EXTCON_DISP_DVI] = {
		.type = EXTCON_TYPE_DISP,
		.id = EXTCON_DISP_DVI,
		.name = "DVI",
	},
	[EXTCON_DISP_VGA] = {
		.type = EXTCON_TYPE_DISP,
		.id = EXTCON_DISP_VGA,
		.name = "VGA",
	},
	[EXTCON_DISP_DP] = {
		.type = EXTCON_TYPE_DISP | EXTCON_TYPE_USB,
		.id = EXTCON_DISP_DP,
		.name = "DP",
	},
	[EXTCON_DISP_HMD] = {
		.type = EXTCON_TYPE_DISP | EXTCON_TYPE_USB,
		.id = EXTCON_DISP_HMD,
		.name = "HMD",
	},

	/* Miscellaneous external connector */
	[EXTCON_DOCK] = {
		.type = EXTCON_TYPE_MISC,
		.id = EXTCON_DOCK,
		.name = "DOCK",
	},
	[EXTCON_JIG] = {
		.type = EXTCON_TYPE_MISC,
		.id = EXTCON_JIG,
		.name = "JIG",
	},
	[EXTCON_MECHANICAL] = {
		.type = EXTCON_TYPE_MISC,
		.id = EXTCON_MECHANICAL,
		.name = "MECHANICAL",
	},

	{ /* sentinel */ }
};

int extcon_get(const char *devname, struct udevice **devp)
{
	return uclass_get_device_by_name(UCLASS_EXTCON, devname, devp);
}

int extcon_get_cable_id(struct udevice *dev, int *type)
{
	const struct dm_extcon_ops *ops = dev_get_driver_ops(dev);
	int id;

	if (!ops || !ops->get_cable_id)
		return -ENOSYS;

	id = ops->get_cable_id(dev);
	if (id < EXTCON_NONE || id >= EXTCON_NUM)
		return id;

	if (type)
		*type = extcon_info[id].type;

	return id;
}

int extcon_get_max_charge_current(struct udevice *dev)
{
	const struct dm_extcon_ops *ops = dev_get_driver_ops(dev);

	if (!ops)
		return -ENOSYS;

	if (!ops->get_max_charge_current)
		/* not an error to not implement this function */
		return 0;

	return ops->get_max_charge_current(dev);
}

const char *extcon_get_cable_name(int id) {
	if (id < EXTCON_NONE || id >= EXTCON_NUM)
		return "Unknown";

	if (extcon_info[id].id != id)
		return "Unknown";

	return extcon_info[id].name;
}

UCLASS_DRIVER(extcon) = {
	.id = UCLASS_EXTCON,
	.name = "extcon",
};
