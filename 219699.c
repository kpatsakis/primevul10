static void ov511_i2c_w(struct sd *sd, u8 reg, u8 value)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int rc, retries;

	gspca_dbg(gspca_dev, D_USBO, "ov511_i2c_w %02x %02x\n", reg, value);

	/* Three byte write cycle */
	for (retries = 6; ; ) {
		/* Select camera register */
		reg_w(sd, R51x_I2C_SADDR_3, reg);

		/* Write "value" to I2C data port of OV511 */
		reg_w(sd, R51x_I2C_DATA, value);

		/* Initiate 3-byte write cycle */
		reg_w(sd, R511_I2C_CTL, 0x01);

		do {
			rc = reg_r(sd, R511_I2C_CTL);
		} while (rc > 0 && ((rc & 1) == 0)); /* Retry until idle */

		if (rc < 0)
			return;

		if ((rc & 2) == 0) /* Ack? */
			break;
		if (--retries < 0) {
			gspca_dbg(gspca_dev, D_USBO, "i2c write retries exhausted\n");
			return;
		}
	}
}