snd_seq_oss_synth_addr(struct seq_oss_devinfo *dp, int dev, struct snd_seq_event *ev)
{
	if (! snd_seq_oss_synth_is_valid(dp, dev))
		return -EINVAL;
	snd_seq_oss_fill_addr(dp, ev, dp->synths[dev].arg.addr.client,
			      dp->synths[dev].arg.addr.port);
	return 0;
}