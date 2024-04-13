get_synthdev(struct seq_oss_devinfo *dp, int dev)
{
	struct seq_oss_synth *rec;
	if (dev < 0 || dev >= dp->max_synthdev)
		return NULL;
	if (! dp->synths[dev].opened)
		return NULL;
	if (dp->synths[dev].is_midi)
		return &midi_synth_dev;
	if ((rec = get_sdev(dev)) == NULL)
		return NULL;
	if (! rec->opened) {
		snd_use_lock_free(&rec->use_lock);
		return NULL;
	}
	return rec;
}