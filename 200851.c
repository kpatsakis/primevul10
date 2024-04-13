static int chip_command(struct i2c_client *client,
			unsigned int cmd, void *arg)
{
	struct CHIPSTATE *chip = i2c_get_clientdata(client);
	struct CHIPDESC  *desc = chip->desc;

	if (debug > 0) {
		v4l_i2c_print_ioctl(chip->c, cmd);
		printk("\n");
	}

	switch (cmd) {
	case AUDC_SET_RADIO:
		chip->radio = 1;
		chip->watch_stereo = 0;
		/* del_timer(&chip->wt); */
		break;
	/* --- v4l ioctls --- */
	/* take care: bttv does userspace copying, we'll get a
	kernel pointer here... */
	case VIDIOC_QUERYCTRL:
	{
		struct v4l2_queryctrl *qc = arg;

		switch (qc->id) {
			case V4L2_CID_AUDIO_MUTE:
				break;
			case V4L2_CID_AUDIO_VOLUME:
			case V4L2_CID_AUDIO_BALANCE:
				if (!(desc->flags & CHIP_HAS_VOLUME))
					return -EINVAL;
				break;
			case V4L2_CID_AUDIO_BASS:
			case V4L2_CID_AUDIO_TREBLE:
				if (!(desc->flags & CHIP_HAS_BASSTREBLE))
					return -EINVAL;
				break;
			default:
				return -EINVAL;
		}
		return v4l2_ctrl_query_fill_std(qc);
	}
	case VIDIOC_S_CTRL:
		return tvaudio_set_ctrl(chip, arg);

	case VIDIOC_G_CTRL:
		return tvaudio_get_ctrl(chip, arg);
	case VIDIOC_INT_G_AUDIO_ROUTING:
	{
		struct v4l2_routing *rt = arg;

		rt->input = chip->input;
		rt->output = 0;
		break;
	}
	case VIDIOC_INT_S_AUDIO_ROUTING:
	{
		struct v4l2_routing *rt = arg;

		if (!(desc->flags & CHIP_HAS_INPUTSEL) || rt->input >= 4)
				return -EINVAL;
		/* There are four inputs: tuner, radio, extern and intern. */
		chip->input = rt->input;
		if (chip->muted)
			break;
		chip_write_masked(chip, desc->inputreg,
				desc->inputmap[chip->input], desc->inputmask);
		break;
	}
	case VIDIOC_S_TUNER:
	{
		struct v4l2_tuner *vt = arg;
		int mode = 0;

		if (chip->radio)
			break;
		switch (vt->audmode) {
		case V4L2_TUNER_MODE_MONO:
		case V4L2_TUNER_MODE_STEREO:
		case V4L2_TUNER_MODE_LANG1:
		case V4L2_TUNER_MODE_LANG2:
			mode = vt->audmode;
			break;
		case V4L2_TUNER_MODE_LANG1_LANG2:
			mode = V4L2_TUNER_MODE_STEREO;
			break;
		default:
			return -EINVAL;
		}
		chip->audmode = vt->audmode;

		if (desc->setmode && mode) {
			chip->watch_stereo = 0;
			/* del_timer(&chip->wt); */
			chip->mode = mode;
			desc->setmode(chip, mode);
		}
		break;
	}
	case VIDIOC_G_TUNER:
	{
		struct v4l2_tuner *vt = arg;
		int mode = V4L2_TUNER_MODE_MONO;

		if (chip->radio)
			break;
		vt->audmode = chip->audmode;
		vt->rxsubchans = 0;
		vt->capability = V4L2_TUNER_CAP_STEREO |
			V4L2_TUNER_CAP_LANG1 | V4L2_TUNER_CAP_LANG2;

		if (desc->getmode)
			mode = desc->getmode(chip);

		if (mode & V4L2_TUNER_MODE_MONO)
			vt->rxsubchans |= V4L2_TUNER_SUB_MONO;
		if (mode & V4L2_TUNER_MODE_STEREO)
			vt->rxsubchans |= V4L2_TUNER_SUB_STEREO;
		/* Note: for SAP it should be mono/lang2 or stereo/lang2.
		   When this module is converted fully to v4l2, then this
		   should change for those chips that can detect SAP. */
		if (mode & V4L2_TUNER_MODE_LANG1)
			vt->rxsubchans = V4L2_TUNER_SUB_LANG1 |
					 V4L2_TUNER_SUB_LANG2;
		break;
	}
	case VIDIOC_S_STD:
		chip->radio = 0;
		break;
	case VIDIOC_S_FREQUENCY:
		chip->mode = 0; /* automatic */

		/* For chips that provide getmode and setmode, and doesn't
		   automatically follows the stereo carrier, a kthread is
		   created to set the audio standard. In this case, when then
		   the video channel is changed, tvaudio starts on MONO mode.
		   After waiting for 2 seconds, the kernel thread is called,
		   to follow whatever audio standard is pointed by the
		   audio carrier.
		 */
		if (chip->thread) {
			desc->setmode(chip,V4L2_TUNER_MODE_MONO);
			if (chip->prevmode != V4L2_TUNER_MODE_MONO)
				chip->prevmode = -1; /* reset previous mode */
			mod_timer(&chip->wt, jiffies+msecs_to_jiffies(2000));
		}
		break;

	case VIDIOC_G_CHIP_IDENT:
		return v4l2_chip_ident_i2c_client(client, arg, V4L2_IDENT_TVAUDIO, 0);
	}
	return 0;
}