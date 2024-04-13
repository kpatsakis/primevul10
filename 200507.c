snd_seq_oss_synth_ioctl(struct seq_oss_devinfo *dp, int dev, unsigned int cmd, unsigned long addr)
{
	struct seq_oss_synth *rec;
	int rc;

	if (is_midi_dev(dp, dev))
		return -ENXIO;
	if ((rec = get_synthdev(dp, dev)) == NULL)
		return -ENXIO;
	if (rec->oper.ioctl == NULL)
		rc = -ENXIO;
	else
		rc = rec->oper.ioctl(&dp->synths[dev].arg, cmd, addr);
	snd_use_lock_free(&rec->use_lock);
	return rc;
}