static void tda985x_setmode(struct CHIPSTATE *chip, int mode)
{
	int update = 1;
	int c6 = chip->shadow.bytes[TDA985x_C6+1] & 0x3f;

	switch (mode) {
	case V4L2_TUNER_MODE_MONO:
		c6 |= TDA985x_MONO;
		break;
	case V4L2_TUNER_MODE_STEREO:
		c6 |= TDA985x_STEREO;
		break;
	case V4L2_TUNER_MODE_LANG1:
		c6 |= TDA985x_SAP;
		break;
	default:
		update = 0;
	}
	if (update)
		chip_write(chip,TDA985x_C6,c6);
}