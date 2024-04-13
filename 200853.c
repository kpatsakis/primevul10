static int chip_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct CHIPSTATE *chip;
	struct CHIPDESC  *desc;

	if (debug) {
		printk(KERN_INFO "tvaudio: TV audio decoder + audio/video mux driver\n");
		printk(KERN_INFO "tvaudio: known chips: ");
		for (desc = chiplist; desc->name != NULL; desc++)
			printk("%s%s", (desc == chiplist) ? "" : ", ", desc->name);
		printk("\n");
	}

	chip = kzalloc(sizeof(*chip),GFP_KERNEL);
	if (!chip)
		return -ENOMEM;
	chip->c = client;
	i2c_set_clientdata(client, chip);

	/* find description for the chip */
	v4l_dbg(1, debug, client, "chip found @ 0x%x\n", client->addr<<1);
	for (desc = chiplist; desc->name != NULL; desc++) {
		if (0 == *(desc->insmodopt))
			continue;
		if (client->addr < desc->addr_lo ||
		    client->addr > desc->addr_hi)
			continue;
		if (desc->checkit && !desc->checkit(chip))
			continue;
		break;
	}
	if (desc->name == NULL) {
		v4l_dbg(1, debug, client, "no matching chip description found\n");
		kfree(chip);
		return -EIO;
	}
	v4l_info(client, "%s found @ 0x%x (%s)\n", desc->name, client->addr<<1, client->adapter->name);
	if (desc->flags) {
		v4l_dbg(1, debug, client, "matches:%s%s%s.\n",
			(desc->flags & CHIP_HAS_VOLUME)     ? " volume"      : "",
			(desc->flags & CHIP_HAS_BASSTREBLE) ? " bass/treble" : "",
			(desc->flags & CHIP_HAS_INPUTSEL)   ? " audiomux"    : "");
	}

	/* fill required data structures */
	if (!id)
		strlcpy(client->name, desc->name, I2C_NAME_SIZE);
	chip->desc = desc;
	chip->shadow.count = desc->registers+1;
	chip->prevmode = -1;
	chip->audmode = V4L2_TUNER_MODE_LANG1;

	/* initialization  */
	if (desc->initialize != NULL)
		desc->initialize(chip);
	else
		chip_cmd(chip,"init",&desc->init);

	if (desc->flags & CHIP_HAS_VOLUME) {
		if (!desc->volfunc) {
			/* This shouldn't be happen. Warn user, but keep working
			   without volume controls
			 */
			v4l_info(chip->c, "volume callback undefined!\n");
			desc->flags &= ~CHIP_HAS_VOLUME;
		} else {
			chip->left  = desc->leftinit  ? desc->leftinit  : 65535;
			chip->right = desc->rightinit ? desc->rightinit : 65535;
			chip_write(chip, desc->leftreg,
				   desc->volfunc(chip->left));
			chip_write(chip, desc->rightreg,
				   desc->volfunc(chip->right));
		}
	}
	if (desc->flags & CHIP_HAS_BASSTREBLE) {
		if (!desc->bassfunc || !desc->treblefunc) {
			/* This shouldn't be happen. Warn user, but keep working
			   without bass/treble controls
			 */
			v4l_info(chip->c, "bass/treble callbacks undefined!\n");
			desc->flags &= ~CHIP_HAS_BASSTREBLE;
		} else {
			chip->treble = desc->trebleinit ?
						desc->trebleinit : 32768;
			chip->bass   = desc->bassinit   ?
						desc->bassinit   : 32768;
			chip_write(chip, desc->bassreg,
				   desc->bassfunc(chip->bass));
			chip_write(chip, desc->treblereg,
				   desc->treblefunc(chip->treble));
		}
	}

	chip->thread = NULL;
	if (desc->flags & CHIP_NEED_CHECKMODE) {
		if (!desc->getmode || !desc->setmode) {
			/* This shouldn't be happen. Warn user, but keep working
			   without kthread
			 */
			v4l_info(chip->c, "set/get mode callbacks undefined!\n");
			return 0;
		}
		/* start async thread */
		init_timer(&chip->wt);
		chip->wt.function = chip_thread_wake;
		chip->wt.data     = (unsigned long)chip;
		chip->thread = kthread_run(chip_thread, chip, chip->c->name);
		if (IS_ERR(chip->thread)) {
			v4l_warn(chip->c, "%s: failed to create kthread\n",
			       chip->c->name);
			chip->thread = NULL;
		}
	}
	return 0;
}