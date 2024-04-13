static void i2c_w(struct sd *sd, u8 reg, u8 value)
{
	if (sd->sensor_reg_cache[reg] == value)
		return;

	switch (sd->bridge) {
	case BRIDGE_OV511:
	case BRIDGE_OV511PLUS:
		ov511_i2c_w(sd, reg, value);
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
	case BRIDGE_OV519:
		ov518_i2c_w(sd, reg, value);
		break;
	case BRIDGE_OVFX2:
		ovfx2_i2c_w(sd, reg, value);
		break;
	case BRIDGE_W9968CF:
		w9968cf_i2c_w(sd, reg, value);
		break;
	}

	if (sd->gspca_dev.usb_err >= 0) {
		/* Up on sensor reset empty the register cache */
		if (reg == 0x12 && (value & 0x80))
			memset(sd->sensor_reg_cache, -1,
				sizeof(sd->sensor_reg_cache));
		else
			sd->sensor_reg_cache[reg] = value;
	}
}