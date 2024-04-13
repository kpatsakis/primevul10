snd_seq_oss_drain_write(struct seq_oss_devinfo *dp)
{
	if (! dp->timer->running)
		return;
	if (is_write_mode(dp->file_mode) && !is_nonblock_mode(dp->file_mode) &&
	    dp->writeq) {
		debug_printk(("syncing..\n"));
		while (snd_seq_oss_writeq_sync(dp->writeq))
			;
	}
}