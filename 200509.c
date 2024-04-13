snd_seq_oss_synth_init(void)
{
	snd_use_lock_init(&midi_synth_dev.use_lock);
}