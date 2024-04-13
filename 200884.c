static int chip_legacy_probe(struct i2c_adapter *adap)
{
	/* don't attach on saa7146 based cards,
	   because dedicated drivers are used */
	if ((adap->id == I2C_HW_SAA7146))
		return 0;
	if (adap->class & I2C_CLASS_TV_ANALOG)
		return 1;
	return 0;
}