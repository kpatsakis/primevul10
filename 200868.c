static int chip_remove(struct i2c_client *client)
{
	struct CHIPSTATE *chip = i2c_get_clientdata(client);

	del_timer_sync(&chip->wt);
	if (chip->thread) {
		/* shutdown async thread */
		kthread_stop(chip->thread);
		chip->thread = NULL;
	}

	kfree(chip);
	return 0;
}