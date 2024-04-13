static int init_ov_sensor(struct sd *sd, u8 slave)
{
	int i;
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;

	ov51x_set_slave_ids(sd, slave);

	/* Reset the sensor */
	i2c_w(sd, 0x12, 0x80);

	/* Wait for it to initialize */
	msleep(150);

	for (i = 0; i < i2c_detect_tries; i++) {
		if (i2c_r(sd, OV7610_REG_ID_HIGH) == 0x7f &&
		    i2c_r(sd, OV7610_REG_ID_LOW) == 0xa2) {
			gspca_dbg(gspca_dev, D_PROBE, "I2C synced in %d attempt(s)\n",
				  i);
			return 0;
		}

		/* Reset the sensor */
		i2c_w(sd, 0x12, 0x80);

		/* Wait for it to initialize */
		msleep(150);

		/* Dummy read to sync I2C */
		if (i2c_r(sd, 0x00) < 0)
			return -1;
	}
	return -1;
}