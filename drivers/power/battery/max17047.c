/*
 * Copyright (C) 2018 Simon Shields <simon@lineageos.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <power/battery.h>
#include <power/pmic.h>
#include <power/max17047.h>

#define CHARACTER_SIZE 16
#define AVG_SAMPLE_COUNT 5

#define MAX17047_EXT_VOLTAGE_THRESH 3850000
#define MAX17047_LOW_VOLTAGE_THRESH 3600000

struct max17047_fg_data {
	u16 character0[CHARACTER_SIZE];
	u16 character1[CHARACTER_SIZE];
	u16 character2[CHARACTER_SIZE];
	u16 rcomp0;
	u16 tempco;
};

static int max17047_i2c_read(struct udevice *dev, int reg, u16 *data)
{
	u8 buf[2] = {0};

	int ret = pmic_read(dev->parent, reg, buf, 2);
	if (ret < 0)
		return ret;

	*data = le16_to_cpu(buf[1] << 8 | buf[0]);

	return 0;
}

static int max17047_i2c_write(struct udevice *dev, int reg, u16 data)
{
	u8 buf[2] = {0};

	buf[1] = cpu_to_le16(data) >> 8;
	buf[0] = cpu_to_le16(data) & 0xff;

	return pmic_write(dev->parent, reg, buf, 2);
}

static int max17047_get_voltage(struct udevice *dev, unsigned int *uV)
{
	u16 data;
	int ret = max17047_i2c_read(dev, MAX17047_VCELL, &data);
	if (ret)
		return ret;
	*uV = 625 * (data >> 3);
	return 0;
}

static u32 max17047_get_average_voltage(struct udevice *dev)
{
	u32 vcell_data;
	u32 vcell_max = 0;
	u32 vcell_min = 0;
	u32 vcell_total = 0;
	int err;

	for (int i = 0; i < AVG_SAMPLE_COUNT; i++) {
		err = max17047_get_voltage(dev, &vcell_data);
		if (err) {
			printf("get_voltage failed: %d\n", err);
			return 0;
		}
		if (i == 0) {
			vcell_min = vcell_max = vcell_data;
		} else {
			if (vcell_data > vcell_max)
				vcell_max = vcell_data;
			else if (vcell_data < vcell_min)
				vcell_min = vcell_data;
		}
		vcell_total += vcell_data;
	}

	vcell_total -= vcell_max;
	vcell_total -= vcell_min;

	return vcell_total / (AVG_SAMPLE_COUNT - 2);
}

static int max17047_get_soc(struct udevice *dev)
{
	u16 data;
	int err = max17047_i2c_read(dev, MAX17047_VFSOC, &data);
	if (err)
		return err;
	return data >> 8;
}

static int max17047_get_status(struct udevice *dev)
{
	u32 voltage;
	int soc;
	int ret = max17047_get_voltage(dev, &voltage);
	if (ret)
		return ret;

	soc = max17047_get_soc(dev);
	if (soc < 0)
		return soc;

	if (voltage > MAX17047_EXT_VOLTAGE_THRESH)
		return BAT_STATE_NOT_PRESENT;
	else if (voltage < MAX17047_LOW_VOLTAGE_THRESH || soc < 5)
		return BAT_STATE_NEED_CHARGING;

	return BAT_STATE_NORMAL;
}

static int max17047_write_array(struct udevice *dev, int reg, u16 *data, int length)
{
	int err;
	for (int i = 0; i < length; i++) {
		err = max17047_i2c_write(dev, reg + i, data[i]);
		if (err)
			return err;
	}

	return 0;
}

static int max17047_read_array(struct udevice *dev, int reg, u16 *data, int length)
{
	int err;
	for (int i = 0; i < length; i++) {
		err = max17047_i2c_read(dev, reg + i, &data[i]);
		if (err < 0) {
			printf("read %#x failed: %d\n", reg + i, err);
			return err;
		}
	}

	return 0;
}

static int max17047_write_and_verify(struct udevice *dev, int reg, u16 data)
{
	int ret = 0;
	ret = max17047_i2c_write(dev, reg, data);
	if (ret)
		return ret;
	ret = max17047_i2c_read(dev, reg, &data);
	if (ret)
		return ret;
	return data;
}

static int max17047_power_on_reset(struct udevice *dev)
{
	struct max17047_fg_data *priv = dev->priv;
	u16 status;
	u16 data0[16];
	u16 data1[16];
	u16 data2[16];
	int write_tries = 5;
	bool ok = true;

	u32 vcell, soc;

	vcell = max17047_get_average_voltage(dev);
	soc = max17047_get_soc(dev);

	debug("%s: vcell: %u, soc: %u\n", __func__, vcell, soc);
	/* delay 500ms */
	mdelay(500);

	max17047_i2c_write(dev, MAX17047_CONFIG, 0x2310);

do_write:
	do {
		ok = true;
		/* Unlock model */
		max17047_i2c_write(dev, MAX17047_MLOCKReg1, 0x59);
		max17047_i2c_write(dev, MAX17047_MLOCKReg2, 0xc4);
		/* Update model */
		max17047_write_array(dev, MAX17047_MODEL1, priv->character0, 16);
		max17047_write_array(dev, MAX17047_MODEL2, priv->character1, 16);
		max17047_write_array(dev, MAX17047_MODEL3, priv->character2, 16);
		/* Check model */
		max17047_read_array(dev, MAX17047_MODEL1, data0, 16);
		max17047_read_array(dev, MAX17047_MODEL2, data1, 16);
		max17047_read_array(dev, MAX17047_MODEL3, data2, 16);

		for (int i = 0; i < 16; i++) {
			if (priv->character0[i] != data0[i]) {
				ok = false;
			}
			if (priv->character1[i] != data1[i]) {
				ok = false;
			}
			if (priv->character2[i] != data2[i]) {
				ok = false;
			}

			if (!ok)
				break;
		}
	} while (!ok && write_tries-- > 0);

	if (!ok) {
		printf("%s: Failed to write model!\n", __func__);
		return -EIO;
	}

	ok = true;
	/* relock model */
	max17047_i2c_write(dev, MAX17047_MLOCKReg1, 0);
	max17047_i2c_write(dev, MAX17047_MLOCKReg2, 0);

	/* Check model was locked - it should be all zero */
	max17047_read_array(dev, MAX17047_MODEL1, data0, 16);
	max17047_read_array(dev, MAX17047_MODEL2, data1, 16);
	max17047_read_array(dev, MAX17047_MODEL3, data2, 16);

	for (int i = 0; i < 16; i++) {
		if (data0[i] || data1[i] || data2[i]) {
			ok = false;
			break;
		}
	}

	if (!ok && write_tries) {
		/* model lock failed, try to rewrite it */
		printf("%s: model lock failed, attempting to rewrite...\n", __func__);
		goto do_write;
	} else if (!ok) {
		printf("%s: model lock failed, ignoring...\n", __func__);
	}

	max17047_write_and_verify(dev, MAX17047_RCOMP0, priv->rcomp0);
	max17047_write_and_verify(dev, MAX17047_TEMPCO, priv->tempco);

	mdelay(350);
	max17047_i2c_read(dev, MAX17047_STATUS, &status);
	status &= 0xfffd;
	max17047_write_and_verify(dev, MAX17047_STATUS, status);

	debug("%s: POR completed successfully. SOC: %d\n", __func__, max17047_get_soc(dev));
	return 0;
}


static int max17047_probe(struct udevice *dev)
{
	int ret;
	u16 status;

	ret = max17047_i2c_read(dev, MAX17047_STATUS, &status);
	if (ret) {
		printf("%s: failed to read status register: %d\n", __func__, ret);
		return ret;
	}

	if (status & MAX17047_STATUS_POR) {
		ret = max17047_power_on_reset(dev);
		if (ret)
			return ret;
	}
	return 0;
}

static int max17047_ofdata_to_platdata(struct udevice *dev)
{
	struct max17047_fg_data *priv = dev->priv;
	u32 data[CHARACTER_SIZE];
	u32 val;
	int err, i;

	err = dev_read_u32_array(dev, "maxim,cell-character0", data, CHARACTER_SIZE);
	if (err < 0) {
		printf("Failed to read cell-character0: %d\n", err);
		return err;
	}

	for (i = 0; i < CHARACTER_SIZE; i++) {
		priv->character0[i] = data[i] & 0xffff;
	}

	err = dev_read_u32_array(dev, "maxim,cell-character1", data, CHARACTER_SIZE);
	if (err < 0) {
		printf("Failed to read cell-character1: %d\n", err);
		return err;
	}

	for (i = 0; i < CHARACTER_SIZE; i++) {
		priv->character1[i] = data[i] & 0xffff;
	}

	err = dev_read_u32_array(dev, "maxim,cell-character2", data, CHARACTER_SIZE);
	if (err < 0) {
		printf("Failed to read cell-character2: %d\n", err);
		return err;
	}

	for (i = 0; i < CHARACTER_SIZE; i++) {
		priv->character2[i] = data[i] & 0xffff;
	}

	err = dev_read_u32(dev, "maxim,rcomp0", &val);
	if (err < 0) {
		printf("Failed to read rcomp0: %d\n", err);
		return err;
	}

	priv->rcomp0 = val & 0xffff;

	err = dev_read_u32(dev, "maxim,tempco", &val);
	if (err < 0) {
		printf("Failed to read tempco: %d\n", err);
		return err;
	}

	priv->tempco = val & 0xffff;

	return 0;
}

static struct dm_battery_ops max17047_battery_ops = {
	.get_voltage = max17047_get_voltage,
	.get_status = max17047_get_status,
	.get_soc = max17047_get_soc,
};

U_BOOT_DRIVER(battery_max17047) = {
	.name = MAX17047_FUELGAUGE_DRIVER,
	.id = UCLASS_BATTERY,
	.ops = &max17047_battery_ops,
	.probe = max17047_probe,
	.ofdata_to_platdata = max17047_ofdata_to_platdata,
	.priv_auto_alloc_size = sizeof(struct max17047_fg_data),
};

