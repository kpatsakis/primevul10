static int i2c_r(struct sd *sd, u8 reg)
{
	int ret = -1;

	if (sd->sensor_reg_cache[reg] != -1)
		return sd->sensor_reg_cache[reg];

	switch (sd->bridge) {
	case BRIDGE_OV511:
	case BRIDGE_OV511PLUS:
		ret = ov511_i2c_r(sd, reg);
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
	case BRIDGE_OV519:
		ret = ov518_i2c_r(sd, reg);
		break;
	case BRIDGE_OVFX2:
		ret = ovfx2_i2c_r(sd, reg);
		break;
	case BRIDGE_W9968CF:
		ret = w9968cf_i2c_r(sd, reg);
		break;
	}

	if (ret >= 0)
		sd->sensor_reg_cache[reg] = ret;

	return ret;
}