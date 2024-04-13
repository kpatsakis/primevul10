static void ta8874z_setmode(struct CHIPSTATE *chip, int mode)
{
	int update = 1;
	audiocmd *t = NULL;
	v4l_dbg(1, debug, chip->c, "ta8874z_setmode(): mode: 0x%02x\n", mode);

	switch(mode){
	case V4L2_TUNER_MODE_MONO:
		t = &ta8874z_mono;
		break;
	case V4L2_TUNER_MODE_STEREO:
		t = &ta8874z_stereo;
		break;
	case V4L2_TUNER_MODE_LANG1:
		t = &ta8874z_main;
		break;
	case V4L2_TUNER_MODE_LANG2:
		t = &ta8874z_sub;
		break;
	default:
		update = 0;
	}

	if(update)
		chip_cmd(chip, "TA8874Z", t);
}