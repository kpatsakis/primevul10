static int tda9873_getmode(struct CHIPSTATE *chip)
{
	int val,mode;

	val = chip_read(chip);
	mode = V4L2_TUNER_MODE_MONO;
	if (val & TDA9873_STEREO)
		mode |= V4L2_TUNER_MODE_STEREO;
	if (val & TDA9873_DUAL)
		mode |= V4L2_TUNER_MODE_LANG1 | V4L2_TUNER_MODE_LANG2;
	v4l_dbg(1, debug, chip->c, "tda9873_getmode(): raw chip read: %d, return: %d\n",
		val, mode);
	return mode;
}