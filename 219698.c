static void ov51x_set_slave_ids(struct sd *sd,
				u8 slave)
{
	switch (sd->bridge) {
	case BRIDGE_OVFX2:
		reg_w(sd, OVFX2_I2C_ADDR, slave);
		return;
	case BRIDGE_W9968CF:
		sd->sensor_addr = slave;
		return;
	}

	reg_w(sd, R51x_I2C_W_SID, slave);
	reg_w(sd, R51x_I2C_R_SID, slave + 1);
}