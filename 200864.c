static int chip_cmd(struct CHIPSTATE *chip, char *name, audiocmd *cmd)
{
	int i;

	if (0 == cmd->count)
		return 0;

	if (cmd->count + cmd->bytes[0] - 1 >= ARRAY_SIZE(chip->shadow.bytes)) {
		v4l_info(chip->c,
			 "Tried to access a non-existent register range: %d to %d\n",
			 cmd->bytes[0] + 1, cmd->bytes[0] + cmd->count - 1);
		return -EINVAL;
	}

	/* FIXME: it seems that the shadow bytes are wrong bellow !*/

	/* update our shadow register set; print bytes if (debug > 0) */
	v4l_dbg(1, debug, chip->c, "%s: chip_cmd(%s): reg=%d, data:",
		chip->c->name, name,cmd->bytes[0]);
	for (i = 1; i < cmd->count; i++) {
		if (debug)
			printk(" 0x%x",cmd->bytes[i]);
		chip->shadow.bytes[i+cmd->bytes[0]] = cmd->bytes[i];
	}
	if (debug)
		printk("\n");

	/* send data to the chip */
	if (cmd->count != i2c_master_send(chip->c,cmd->bytes,cmd->count)) {
		v4l_warn(chip->c, "%s: I/O error (%s)\n", chip->c->name, name);
		return -1;
	}
	return 0;
}