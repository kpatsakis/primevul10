is_midi_dev(struct seq_oss_devinfo *dp, int dev)
{
	if (dev < 0 || dev >= dp->max_synthdev)
		return 0;
	if (dp->synths[dev].is_midi)
		return 1;
	return 0;
}