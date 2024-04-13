static void tda9873_setmode(struct CHIPSTATE *chip, int mode)
{
	int sw_data  = chip->shadow.bytes[TDA9873_SW+1] & ~ TDA9873_TR_MASK;
	/*	int adj_data = chip->shadow.bytes[TDA9873_AD+1] ; */

	if ((sw_data & TDA9873_INP_MASK) != TDA9873_INTERNAL) {
		v4l_dbg(1, debug, chip->c, "tda9873_setmode(): external input\n");
		return;
	}

	v4l_dbg(1, debug, chip->c, "tda9873_setmode(): chip->shadow.bytes[%d] = %d\n", TDA9873_SW+1, chip->shadow.bytes[TDA9873_SW+1]);
	v4l_dbg(1, debug, chip->c, "tda9873_setmode(): sw_data  = %d\n", sw_data);

	switch (mode) {
	case V4L2_TUNER_MODE_MONO:
		sw_data |= TDA9873_TR_MONO;
		break;
	case V4L2_TUNER_MODE_STEREO:
		sw_data |= TDA9873_TR_STEREO;
		break;
	case V4L2_TUNER_MODE_LANG1:
		sw_data |= TDA9873_TR_DUALA;
		break;
	case V4L2_TUNER_MODE_LANG2:
		sw_data |= TDA9873_TR_DUALB;
		break;
	default:
		chip->mode = 0;
		return;
	}

	chip_write(chip, TDA9873_SW, sw_data);
	v4l_dbg(1, debug, chip->c, "tda9873_setmode(): req. mode %d; chip_write: %d\n",
		mode, sw_data);
}