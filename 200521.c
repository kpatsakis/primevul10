snd_seq_oss_synth_is_valid(struct seq_oss_devinfo *dp, int dev)
{
	struct seq_oss_synth *rec;
	rec = get_synthdev(dp, dev);
	if (rec) {
		snd_use_lock_free(&rec->use_lock);
		return 1;
	}
	return 0;
}