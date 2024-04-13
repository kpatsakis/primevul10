static int ov518_i2c_r(struct sd *sd, u8 reg)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int value;

	/* Select camera register */
	reg_w(sd, R51x_I2C_SADDR_2, reg);

	/* Initiate 2-byte write cycle */
	reg_w(sd, R518_I2C_CTL, 0x03);
	reg_r8(sd, R518_I2C_CTL);

	/* Initiate 2-byte read cycle */
	reg_w(sd, R518_I2C_CTL, 0x05);
	reg_r8(sd, R518_I2C_CTL);

	value = reg_r(sd, R51x_I2C_DATA);
	gspca_dbg(gspca_dev, D_USBI, "ov518_i2c_r %02x %02x\n", reg, value);
	return value;
}