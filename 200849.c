static void tda8425_setmode(struct CHIPSTATE *chip, int mode)
{
	int s1 = chip->shadow.bytes[TDA8425_S1+1] & 0xe1;

	if (mode & V4L2_TUNER_MODE_LANG1) {
		s1 |= TDA8425_S1_ML_SOUND_A;
		s1 |= TDA8425_S1_STEREO_PSEUDO;

	} else if (mode & V4L2_TUNER_MODE_LANG2) {
		s1 |= TDA8425_S1_ML_SOUND_B;
		s1 |= TDA8425_S1_STEREO_PSEUDO;

	} else {
		s1 |= TDA8425_S1_ML_STEREO;

		if (mode & V4L2_TUNER_MODE_MONO)
			s1 |= TDA8425_S1_STEREO_MONO;
		if (mode & V4L2_TUNER_MODE_STEREO)
			s1 |= TDA8425_S1_STEREO_SPATIAL;
	}
	chip_write(chip,TDA8425_S1,s1);
}