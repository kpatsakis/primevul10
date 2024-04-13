static void ov518_i2c_w(struct sd *sd,
		u8 reg,
		u8 value)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;

	gspca_dbg(gspca_dev, D_USBO, "ov518_i2c_w %02x %02x\n", reg, value);

	/* Select camera register */
	reg_w(sd, R51x_I2C_SADDR_3, reg);

	/* Write "value" to I2C data port of OV511 */
	reg_w(sd, R51x_I2C_DATA, value);

	/* Initiate 3-byte write cycle */
	reg_w(sd, R518_I2C_CTL, 0x01);

	/* wait for write complete */
	msleep(4);
	reg_r8(sd, R518_I2C_CTL);
}