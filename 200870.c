static int chip_thread(void *data)
{
	struct CHIPSTATE *chip = data;
	struct CHIPDESC  *desc = chip->desc;
	int mode;

	v4l_dbg(1, debug, chip->c, "%s: thread started\n", chip->c->name);
	set_freezable();
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (!kthread_should_stop())
			schedule();
		set_current_state(TASK_RUNNING);
		try_to_freeze();
		if (kthread_should_stop())
			break;
		v4l_dbg(1, debug, chip->c, "%s: thread wakeup\n", chip->c->name);

		/* don't do anything for radio or if mode != auto */
		if (chip->radio || chip->mode != 0)
			continue;

		/* have a look what's going on */
		mode = desc->getmode(chip);
		if (mode == chip->prevmode)
			continue;

		/* chip detected a new audio mode - set it */
		v4l_dbg(1, debug, chip->c, "%s: thread checkmode\n",
			chip->c->name);

		chip->prevmode = mode;

		if (mode & V4L2_TUNER_MODE_STEREO)
			desc->setmode(chip, V4L2_TUNER_MODE_STEREO);
		if (mode & V4L2_TUNER_MODE_LANG1_LANG2)
			desc->setmode(chip, V4L2_TUNER_MODE_STEREO);
		else if (mode & V4L2_TUNER_MODE_LANG1)
			desc->setmode(chip, V4L2_TUNER_MODE_LANG1);
		else if (mode & V4L2_TUNER_MODE_LANG2)
			desc->setmode(chip, V4L2_TUNER_MODE_LANG2);
		else
			desc->setmode(chip, V4L2_TUNER_MODE_MONO);

		/* schedule next check */
		mod_timer(&chip->wt, jiffies+msecs_to_jiffies(2000));
	}

	v4l_dbg(1, debug, chip->c, "%s: thread exiting\n", chip->c->name);
	return 0;
}