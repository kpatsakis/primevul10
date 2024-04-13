static int chip_read2(struct CHIPSTATE *chip, int subaddr)
{
	unsigned char write[1];
	unsigned char read[1];
	struct i2c_msg msgs[2] = {
		{ chip->c->addr, 0,        1, write },
		{ chip->c->addr, I2C_M_RD, 1, read  }
	};
	write[0] = subaddr;

	if (2 != i2c_transfer(chip->c->adapter,msgs,2)) {
		v4l_warn(chip->c, "%s: I/O error (read2)\n", chip->c->name);
		return -1;
	}
	v4l_dbg(1, debug, chip->c, "%s: chip_read2: reg%d=0x%x\n",
		chip->c->name, subaddr,read[0]);
	return read[0];
}