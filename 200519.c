snd_seq_oss_synth_setup_midi(struct seq_oss_devinfo *dp)
{
	int i;

	if (dp->max_synthdev >= SNDRV_SEQ_OSS_MAX_SYNTH_DEVS)
		return;

	for (i = 0; i < dp->max_mididev; i++) {
		struct seq_oss_synthinfo *info;
		info = &dp->synths[dp->max_synthdev];
		if (snd_seq_oss_midi_open(dp, i, dp->file_mode) < 0)
			continue;
		info->arg.app_index = dp->port;
		info->arg.file_mode = dp->file_mode;
		info->arg.seq_mode = dp->seq_mode;
		info->arg.private_data = info;
		info->is_midi = 1;
		info->midi_mapped = i;
		info->arg.event_passing = SNDRV_SEQ_OSS_PASS_EVENTS;
		snd_seq_oss_midi_get_addr(dp, i, &info->arg.addr);
		info->opened = 1;
		midi_synth_dev.opened++;
		dp->max_synthdev++;
		if (dp->max_synthdev >= SNDRV_SEQ_OSS_MAX_SYNTH_DEVS)
			break;
	}
}