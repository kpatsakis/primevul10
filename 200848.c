static int chip_read(struct CHIPSTATE *chip)
{
	unsigned char buffer;

	if (1 != i2c_master_recv(chip->c,&buffer,1)) {
		v4l_warn(chip->c, "%s: I/O error (read)\n",
		chip->c->name);
		return -1;
	}
	v4l_dbg(1, debug, chip->c, "%s: chip_read: 0x%x\n",chip->c->name, buffer);
	return buffer;
}