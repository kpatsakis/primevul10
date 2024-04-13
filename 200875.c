static int  tda985x_getmode(struct CHIPSTATE *chip)
{
	int mode;

	mode = ((TDA985x_STP | TDA985x_SAPP) &
		chip_read(chip)) >> 4;
	/* Add mono mode regardless of SAP and stereo */
	/* Allows forced mono */
	return mode | V4L2_TUNER_MODE_MONO;
}