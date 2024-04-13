static int ta8874z_getmode(struct CHIPSTATE *chip)
{
	int val, mode;

	val = chip_read(chip);
	mode = V4L2_TUNER_MODE_MONO;
	if (val & TA8874Z_B1){
		mode |= V4L2_TUNER_MODE_LANG1 | V4L2_TUNER_MODE_LANG2;
	}else if (!(val & TA8874Z_B0)){
		mode |= V4L2_TUNER_MODE_STEREO;
	}
	/* v4l_dbg(1, debug, chip->c, "ta8874z_getmode(): raw chip read: 0x%02x, return: 0x%02x\n", val, mode); */
	return mode;
}