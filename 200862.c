static int tda9840_getmode(struct CHIPSTATE *chip)
{
	int val, mode;

	val = chip_read(chip);
	mode = V4L2_TUNER_MODE_MONO;
	if (val & TDA9840_DS_DUAL)
		mode |= V4L2_TUNER_MODE_LANG1 | V4L2_TUNER_MODE_LANG2;
	if (val & TDA9840_ST_STEREO)
		mode |= V4L2_TUNER_MODE_STEREO;

	v4l_dbg(1, debug, chip->c, "tda9840_getmode(): raw chip read: %d, return: %d\n",
		val, mode);
	return mode;
}