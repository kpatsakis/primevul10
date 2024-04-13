snd_seq_oss_synth_load_patch(struct seq_oss_devinfo *dp, int dev, int fmt,
			    const char __user *buf, int p, int c)
{
	struct seq_oss_synth *rec;
	int rc;

	if (dev < 0 || dev >= dp->max_synthdev)
		return -ENXIO;

	if (is_midi_dev(dp, dev))
		return 0;
	if ((rec = get_synthdev(dp, dev)) == NULL)
		return -ENXIO;

	if (rec->oper.load_patch == NULL)
		rc = -ENXIO;
	else
		rc = rec->oper.load_patch(&dp->synths[dev].arg, fmt, buf, p, c);
	snd_use_lock_free(&rec->use_lock);
	return rc;
}