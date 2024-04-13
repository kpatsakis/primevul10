snd_seq_oss_synth_raw_event(struct seq_oss_devinfo *dp, int dev, unsigned char *data, struct snd_seq_event *ev)
{
	if (! snd_seq_oss_synth_is_valid(dp, dev) || is_midi_dev(dp, dev))
		return -ENXIO;
	ev->type = SNDRV_SEQ_EVENT_OSS;
	memcpy(ev->data.raw8.d, data, 8);
	return snd_seq_oss_synth_addr(dp, dev, ev);
}