free_devinfo(void *private)
{
	struct seq_oss_devinfo *dp = (struct seq_oss_devinfo *)private;

	if (dp->timer)
		snd_seq_oss_timer_delete(dp->timer);
		
	if (dp->writeq)
		snd_seq_oss_writeq_delete(dp->writeq);

	if (dp->readq)
		snd_seq_oss_readq_delete(dp->readq);
	
	kfree(dp);
}