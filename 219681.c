static int ov511_i2c_r(struct sd *sd, u8 reg)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int rc, value, retries;

	/* Two byte write cycle */
	for (retries = 6; ; ) {
		/* Select camera register */
		reg_w(sd, R51x_I2C_SADDR_2, reg);

		/* Initiate 2-byte write cycle */
		reg_w(sd, R511_I2C_CTL, 0x03);

		do {
			rc = reg_r(sd, R511_I2C_CTL);
		} while (rc > 0 && ((rc & 1) == 0)); /* Retry until idle */

		if (rc < 0)
			return rc;

		if ((rc & 2) == 0) /* Ack? */
			break;

		/* I2C abort */
		reg_w(sd, R511_I2C_CTL, 0x10);

		if (--retries < 0) {
			gspca_dbg(gspca_dev, D_USBI, "i2c write retries exhausted\n");
			return -1;
		}
	}

	/* Two byte read cycle */
	for (retries = 6; ; ) {
		/* Initiate 2-byte read cycle */
		reg_w(sd, R511_I2C_CTL, 0x05);

		do {
			rc = reg_r(sd, R511_I2C_CTL);
		} while (rc > 0 && ((rc & 1) == 0)); /* Retry until idle */

		if (rc < 0)
			return rc;

		if ((rc & 2) == 0) /* Ack? */
			break;

		/* I2C abort */
		reg_w(sd, R511_I2C_CTL, 0x10);

		if (--retries < 0) {
			gspca_dbg(gspca_dev, D_USBI, "i2c read retries exhausted\n");
			return -1;
		}
	}

	value = reg_r(sd, R51x_I2C_DATA);

	gspca_dbg(gspca_dev, D_USBI, "ov511_i2c_r %02x %02x\n", reg, value);

	/* This is needed to make i2c_w() work */
	reg_w(sd, R511_I2C_CTL, 0x05);

	return value;
}