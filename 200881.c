static void tda9840_setmode(struct CHIPSTATE *chip, int mode)
{
	int update = 1;
	int t = chip->shadow.bytes[TDA9840_SW + 1] & ~0x7e;

	switch (mode) {
	case V4L2_TUNER_MODE_MONO:
		t |= TDA9840_MONO;
		break;
	case V4L2_TUNER_MODE_STEREO:
		t |= TDA9840_STEREO;
		break;
	case V4L2_TUNER_MODE_LANG1:
		t |= TDA9840_DUALA;
		break;
	case V4L2_TUNER_MODE_LANG2:
		t |= TDA9840_DUALB;
		break;
	default:
		update = 0;
	}

	if (update)
		chip_write(chip, TDA9840_SW, t);
}