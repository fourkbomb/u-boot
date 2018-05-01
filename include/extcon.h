/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _INCLUDE_MUIC_H
#define _INCLUDE_MUIC_H

#include <linux/bitops.h>

/* Following the Linux kernel */
/*
 * Define the type of supported external connectors
 */
#define EXTCON_TYPE_USB		BIT(0)	/* USB connector */
#define EXTCON_TYPE_CHG		BIT(1)	/* Charger connector */
#define EXTCON_TYPE_JACK	BIT(2)	/* Jack connector */
#define EXTCON_TYPE_DISP	BIT(3)	/* Display connector */
#define EXTCON_TYPE_MISC	BIT(4)	/* Miscellaneous connector */

/*
 * Define the unique id of supported external connectors
 */
#define EXTCON_NONE		0

/* USB external connector */
#define EXTCON_USB		1
#define EXTCON_USB_HOST		2

/*
 * Charging external connector
 *
 * When one SDP charger connector was reported, we should also report
 * the USB connector, which means EXTCON_CHG_USB_SDP should always
 * appear together with EXTCON_USB. The same as ACA charger connector,
 * EXTCON_CHG_USB_ACA would normally appear with EXTCON_USB_HOST.
 *
 * The EXTCON_CHG_USB_SLOW connector can provide at least 500mA of
 * current at 5V. The EXTCON_CHG_USB_FAST connector can provide at
 * least 1A of current at 5V.
 */
#define EXTCON_CHG_USB_SDP	5	/* Standard Downstream Port */
#define EXTCON_CHG_USB_DCP	6	/* Dedicated Charging Port */
#define EXTCON_CHG_USB_CDP	7	/* Charging Downstream Port */
#define EXTCON_CHG_USB_ACA	8	/* Accessory Charger Adapter */
#define EXTCON_CHG_USB_FAST	9
#define EXTCON_CHG_USB_SLOW	10
#define EXTCON_CHG_WPT		11	/* Wireless Power Transfer */
#define EXTCON_CHG_USB_PD	12	/* USB Power Delivery */

/* Jack external connector */
#define EXTCON_JACK_MICROPHONE	20
#define EXTCON_JACK_HEADPHONE	21
#define EXTCON_JACK_LINE_IN	22
#define EXTCON_JACK_LINE_OUT	23
#define EXTCON_JACK_VIDEO_IN	24
#define EXTCON_JACK_VIDEO_OUT	25
#define EXTCON_JACK_SPDIF_IN	26	/* Sony Philips Digital InterFace */
#define EXTCON_JACK_SPDIF_OUT	27

/* Display external connector */
#define EXTCON_DISP_HDMI	40	/* High-Definition Multimedia Interface */
#define EXTCON_DISP_MHL		41	/* Mobile High-Definition Link */
#define EXTCON_DISP_DVI		42	/* Digital Visual Interface */
#define EXTCON_DISP_VGA		43	/* Video Graphics Array */
#define EXTCON_DISP_DP		44	/* Display Port */
#define EXTCON_DISP_HMD		45	/* Head-Mounted Display */

/* Miscellaneous external connector */
#define EXTCON_DOCK		60
#define EXTCON_JIG		61
#define EXTCON_MECHANICAL	62

#define EXTCON_NUM		63

/* MUIC device operations */
struct dm_extcon_ops {
	/**
	 * Get the current cable id.
	 * @dev		- extcon device
	 * @return cable ID, EXTCON_NONE <= cable ID < EXTCON_NUM
	 */
	int (*get_cable_id)(struct udevice *dev);
	/**
	 * Get maximum charging current support by currently attached cable.
	 * This function may be NULL if that doesn't make sense for this connector.
	 * The value returned here may not necessarily correspond to the maximum
	 * input current the device itself is able to take - that may depend
	 * on other factors.
	 * @dev		- extcon device
	 * @return  - Maximum charging current supported by the currently attached cable in uA,
	 * or zero if the cable doesn't support charging.
	 */
	int (*get_max_charge_current)(struct udevice *udev);
};

int extcon_get(const char *devname, struct udevice **devp);
int extcon_get_cable_id(struct udevice *dev, int *type);
int extcon_get_max_charge_current(struct udevice *udev);
const char *extcon_get_cable_name(int id);
#endif
